/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-18     Jesven       first version
 * 2021-02-03     lizhirui     port to riscv64
 * 2021-02-06     lizhirui     add thread filter
 * 2021-02-19     lizhirui     port to new version of rt-smart
 * 2021-03-02     lizhirui     add a auxillary function for interrupt
 * 2021-03-04     lizhirui     delete thread filter
 * 2021-03-04     lizhirui     modify for new version of rt-smart
 * 2021-11-22     JasonHu      add lwp_set_thread_context
 * 2021-11-30     JasonHu      add clone/fork support
 */

#include <rthw.h>
#include <stddef.h>

#ifdef RT_USING_USERSPACE

#include <mmu.h>
#include <page.h>
#include <lwp_mm_area.h>
#include <lwp_user_mm.h>
#include <lwp_arch.h>

#include <stack.h>
#include <cpuport.h>
#include <encoding.h>

extern rt_ubase_t MMUTable[];

int arch_expand_user_stack(void *addr)
{
    int ret = 0;
    rt_ubase_t stack_addr = (rt_ubase_t)addr;

    stack_addr &= ~PAGE_OFFSET_MASK;
    if ((stack_addr >= (rt_ubase_t)USER_STACK_VSTART) && (stack_addr < (rt_ubase_t)USER_STACK_VEND))
    {
        void *map = lwp_map_user(lwp_self(), (void *)stack_addr, PAGE_SIZE, RT_FALSE);

        if (map || lwp_user_accessable(addr, 1))
        {
            ret = 1;
        }
    }
    return ret;
}

void *lwp_copy_return_code_to_user_stack()
{
    void lwp_thread_return();
    void lwp_thread_return_end();
    rt_thread_t tid = rt_thread_self();

    if (tid->user_stack != RT_NULL)
    {
        rt_size_t size = (rt_size_t)lwp_thread_return_end - (rt_size_t)lwp_thread_return;
        rt_size_t userstack = (rt_size_t)tid->user_stack + tid->user_stack_size - size;
        rt_memcpy((void *)userstack, lwp_thread_return, size);
        return (void *)userstack;
    }

    return RT_NULL;
}

rt_mmu_info* arch_kernel_get_mmu_info(void)
{
    extern rt_mmu_info *mmu_info;

    return mmu_info;
}

rt_ubase_t lwp_fix_sp(rt_ubase_t cursp)
{
    void lwp_thread_return();
    void lwp_thread_return_end();

    if (cursp == 0)
    {
        return 0;
    }

    return cursp - ((rt_size_t)lwp_thread_return_end - (rt_size_t)lwp_thread_return);
}

rt_thread_t rt_thread_sp_to_thread(void *spmember_addr)
{
    return (rt_thread_t)(((rt_ubase_t)spmember_addr) - (offsetof(struct rt_thread, sp)));
}

void *get_thread_kernel_stack_top(rt_thread_t thread)
{
    return (void *)(((rt_size_t)thread->stack_addr) + ((rt_size_t)thread->stack_size));
}

void *lwp_get_user_sp(void)
{
    /* user sp saved in interrupt context */
    rt_thread_t self = rt_thread_self();
    rt_uint8_t *stack_top = (rt_uint8_t *)self->stack_addr + self->stack_size;
    struct rt_hw_stack_frame *frame = (struct rt_hw_stack_frame *)(stack_top - sizeof(struct rt_hw_stack_frame));

    return (void *)frame->user_sp_exc_stack;
}

int arch_user_space_init(struct rt_lwp *lwp)
{
    rt_ubase_t *mmu_table;

    mmu_table = (rt_ubase_t *)rt_pages_alloc(0);
    if (!mmu_table)
    {
        return -1;
    }

    lwp->end_heap = USER_HEAP_VADDR;

    rt_memcpy(mmu_table, MMUTable, PAGE_SIZE);
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, mmu_table, 4 * PAGE_SIZE);
    rt_hw_mmu_map_init(&lwp->mmu_info, (void *)USER_VADDR_START, USER_VADDR_TOP - USER_VADDR_START, (rt_size_t *)mmu_table, 0);

    return 0;
}

void *arch_kernel_mmu_table_get(void)
{
    return (void *)((char *)MMUTable);
}

void arch_user_space_vtable_free(struct rt_lwp *lwp)
{
    if (lwp && lwp->mmu_info.vtable)
    {
        rt_pages_free(lwp->mmu_info.vtable, 0);
    }
}

long _sys_clone(void *arg[]);
long sys_clone(void *arg[])
{
    return _sys_clone(arg);
}

long _sys_fork(void);
long sys_fork(void)
{
    return _sys_fork();
}

long _sys_vfork(void);
long sys_vfork(void)
{
    return _sys_fork();
}

/**
 * set exec context for fork/clone.
 */
void lwp_set_thread_context(void *exit_addr, void *new_thread_stack, void *user_stack, void **thread_sp)
{
    RT_ASSERT(exit_addr != RT_NULL);
    RT_ASSERT(user_stack != RT_NULL);
    RT_ASSERT(new_thread_stack != RT_NULL);
    RT_ASSERT(thread_sp != RT_NULL);
    struct rt_hw_stack_frame *syscall_frame;
    struct rt_hw_stack_frame *thread_frame;

    rt_uint8_t *stk;
    rt_uint8_t *syscall_stk;

    stk = (rt_uint8_t *)new_thread_stack;
    /* reserve syscall context, all the registers are copyed from parent */
    stk -= CTX_REG_NR * REGBYTES;
    syscall_stk = stk;

    syscall_frame = (struct rt_hw_stack_frame *)stk;

    /* modify user sp */
    syscall_frame->user_sp_exc_stack = (rt_ubase_t)user_stack;

    /* skip ecall */
    syscall_frame->epc += 4;

    /* child return value is 0 */
    syscall_frame->a0 = 0;
    syscall_frame->a1 = 0;

    /* build temp thread context */
    stk -= sizeof(struct rt_hw_stack_frame);

    thread_frame = (struct rt_hw_stack_frame *)stk;

    int i;
    for (i = 0; i < sizeof(struct rt_hw_stack_frame) / sizeof(rt_ubase_t); i++)
    {
        ((rt_ubase_t *)thread_frame)[i] = 0xdeadbeaf;
    }

    /* set pc for thread */
    thread_frame->epc     = (rt_ubase_t)exit_addr;

    /* set old exception mode as supervisor, because in kernel */
    thread_frame->sstatus = read_csr(sstatus) | SSTATUS_SPP;
    thread_frame->sstatus &= ~SSTATUS_SIE; /* must disable interrupt */

    /* set stack as syscall stack */
    thread_frame->user_sp_exc_stack = (rt_ubase_t)syscall_stk;

    /* save new stack top */
    *thread_sp = (void *)stk;

    /**
     * The stack for child thread:
     *
     * +------------------------+ --> kernel stack top
     * | syscall stack          |
     * |                        |
     * | @sp                    | --> `user_stack`
     * | @epc                   | --> user ecall addr + 4 (skip ecall)
     * | @a0&a1                 | --> 0 (for child return 0)
     * |                        |
     * +------------------------+ --> temp thread stack top
     * | temp thread stack      |           ^
     * |                        |           |
     * | @sp                    | ---------/
     * | @epc                   | --> `exit_addr` (sys_clone_exit/sys_fork_exit)
     * |                        |
     * +------------------------+ --> thread sp
     */
}

#endif
