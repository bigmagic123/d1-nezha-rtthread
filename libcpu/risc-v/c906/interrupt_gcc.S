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
    SAVE_ALL

    RESTORE_SYS_GP

    csrr a0, mcause
    csrrc a1, mtval, zero
    csrr  a2, mepc
    mv    a3, sp

    /* mcause, mtval, mepc, sp */
    call  handle_trap

    RESTORE_ALL

    mret

.global rt_hw_interrupt_enable
rt_hw_interrupt_enable:
    csrs mstatus, a0    /* restore to old csr */
    jr ra

.global rt_hw_interrupt_disable
rt_hw_interrupt_disable:
    csrrci a0, mstatus, 8  /*  clear SIE */
    jr ra
