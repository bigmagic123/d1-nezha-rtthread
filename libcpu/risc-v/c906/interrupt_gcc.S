/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/02     Bernard      The first version
 * 2018/12/27     Jesven       Add SMP schedule
 * 2021/02/02     lizhirui     Add userspace support
 */

#define __ASSEMBLY__
#include "cpuport.h"
#include "encoding.h"
#include "stackframe.h"

  .section      .text.entry
  .align 2
  .global trap_entry
  .extern __stack_cpu0
  .extern get_current_thread_kernel_stack_top
trap_entry:
    //backup sp
    //csrrci a0, mstatus, 8
    CLOSE_INTERRUPT
    csrrw sp, mscratch, sp
    //load interrupt stack
    la sp, __stack_cpu0
    //backup context
    SAVE_ALL
    
    RESTORE_SYS_GP

    csrr a0, mcause
    csrrc a1, mtval, zero
    csrr  a2, mepc
    mv    a3, sp

    /* mcause, mtval, mepc, sp */
    call  handle_trap

    RESTORE_ALL

    //csrr t0, mscratch
    //csrw mscratch, sp 
    //STORE t0, 32 * REGBYTES(sp)

    //csrrw	sp,mscratch,sp
    //csrrci	sp,mscratch,0
    csrr sp,mscratch
    OPEN_INTERRUPT
    mret

syscall_entry:
    //swap to thread kernel stack
    csrr t0, sstatus
    andi t0, t0, 0x100
    beqz t0, __restore_sp_from_tcb

__restore_sp_from_sscratch:
    csrr t0, sscratch
    j __move_stack_context

__restore_sp_from_tcb:
    la a0, rt_current_thread
    LOAD a0, 0(a0)
    /*jal get_thread_kernel_stack_top*/
    mv t0, a0

__move_stack_context:
    mv t1, sp//src
    mv sp, t0//switch stack
    addi sp, sp, -CTX_REG_NR * REGBYTES
    //copy context
    li s0, CTX_REG_NR//cnt
    mv t2, sp//dst

copy_context_loop:
    LOAD t0, 0(t1)
    STORE t0, 0(t2)
    addi s0, s0, -1
    addi t1, t1, 8
    addi t2, t2, 8
    bnez s0, copy_context_loop

    LOAD s0, 7 * REGBYTES(sp)
    addi s0, s0, -0xfe
    /* beqz s0, lwp_signal_quit */

    mv a0, sp
    OPEN_INTERRUPT
    call syscall_handler
    CLOSE_INTERRUPT

.global syscall_exit
syscall_exit:

    #if defined(RT_USING_USERSPACE) && defined(RT_USING_SIGNALS)
        LOAD s0, 2 * REGBYTES(sp)
        andi s0, s0, 0x100
        bnez s0, dont_ret_to_user
        li s0, 0
        j ret_to_user
        dont_ret_to_user:
    #endif

    //restore context
    RESTORE_ALL
    sret

.global rt_hw_interrupt_enable
rt_hw_interrupt_enable:
    csrs mstatus, a0    /* restore to old csr */
    jr ra

.global rt_hw_interrupt_disable
rt_hw_interrupt_disable:
    csrrci a0, mstatus, 8  /*  clear SIE */
    jr ra
