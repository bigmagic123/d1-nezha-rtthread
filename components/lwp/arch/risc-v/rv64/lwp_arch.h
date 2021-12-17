/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef  LWP_ARCH_H__
#define  LWP_ARCH_H__

#include <lwp.h>

#ifdef RT_USING_USERSPACE

#ifdef RT_USING_USERSPACE_32BIT_LIMIT
#define USER_HEAP_VADDR     0xF0000000UL
#define USER_HEAP_VEND      0xFE000000UL
#define USER_STACK_VSTART   0xE0000000UL
#define USER_STACK_VEND     USER_HEAP_VADDR
#define USER_VADDR_START    0xC0000000UL
#define USER_VADDR_TOP      0xFF000000UL
#define USER_LOAD_VADDR     0xD0000000UL
#define LDSO_LOAD_VADDR     USER_LOAD_VADDR
#else
#define USER_HEAP_VADDR     0x300000000UL
#define USER_HEAP_VEND      0xffffffffffff0000UL
#define USER_STACK_VSTART   0x270000000UL
#define USER_STACK_VEND     USER_HEAP_VADDR
#define USER_VADDR_START    0x100000000UL
#define USER_VADDR_TOP      0xfffffffffffff000UL
#define USER_LOAD_VADDR     0x200000000
#define LDSO_LOAD_VADDR     0x200000000
#endif

#define MMU_MAP_U_RWCB 0
#define MMU_MAP_U_RW 0

#ifdef __cplusplus
extern "C" {
#endif

int arch_user_space_init(struct rt_lwp *lwp);
void arch_user_space_vtable_free(struct rt_lwp *lwp);
void *arch_kernel_mmu_table_get(void);
void arch_kuser_init(rt_mmu_info *mmu_info, void *vectors);
int arch_expand_user_stack(void *addr);

rt_mmu_info* arch_kernel_get_mmu_info(void);

rt_inline unsigned long ffz(unsigned long x)
{
    return __builtin_ffs(~x) - 1;
}

#ifdef __cplusplus
}
#endif

#endif

#endif  /*LWP_ARCH_H__*/
