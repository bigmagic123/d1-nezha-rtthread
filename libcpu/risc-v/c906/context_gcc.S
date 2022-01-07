/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/28     Bernard      The unify RISC-V porting implementation
 * 2018/12/27     Jesven       Add SMP support
 * 2021/02/02     lizhirui     Add userspace support
 */

#define __ASSEMBLY__
#include "cpuport.h"
#include "stackframe.h"

    .globl rt_hw_context_switch_to
rt_hw_context_switch_to:
    LOAD sp, (a0)

    //la s0, rt_current_thread
    //LOAD s1, (s0)

    RESTORE_ALL
    mret

/*
 * void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to);
 *
 * a0 --> from
 * a1 --> to
 */
    .globl rt_hw_context_switch
rt_hw_context_switch:
    li t0, (0x7880 | 0x00800000)//set SPIE and SPP = 1
    csrs mstatus, t0//if enter here,caller must be in system thread
    csrw mepc, ra//return address
    //saved from thread context
    SAVE_ALL

    STORE sp, (a0)

    //restore to thread context
    LOAD sp, (a1)

    //la s0, rt_current_thread
    //LOAD s1, (s0)

    RESTORE_ALL
    mret
