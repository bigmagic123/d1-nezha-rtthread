/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/01     Bernard      The first version
 * 2018/12/27     Jesven       Add SMP support
 * 2020/6/12      Xim          Port to QEMU and remove SMP support
 */

#define __ASSEMBLY__
#define SSTATUS_FS      0x00006000U /* initial state of FPU, clear to disable */
#include <cpuport.h>

#define REGBYTESX        4
/*mxstatus*/
#define    EN_THEADISAEE    (0x1 << 22)

  .global	_start
  .section ".start", "ax"
_start:

 /*disable interrupt*/
    csrw mie, zero

    /*enable theadisaee*/
    li t1, EN_THEADISAEE | (1 << 15)
    csrs mxstatus, t1

    /*invaild ICACHE/DCACHE/BTB/BHT*/
    li t2, 0x30013
    csrs mcor, t2

    /* irq entry */
    la t0, trap_entry
    csrw mtvec, t0

    la sp, __stack_cpu0
    jal clear_bss
    jal  entry
    j .

clear_bss:
    la    t0, __bss_start
    la    t1, __bss_end

clbss_1:
    sw zero, 0(t0)
    addi t0, t0, REGBYTESX
    blt t0, t1, clbss_1
    ret