/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-19     JasonHu      first version
 */

#ifndef __RISCV64_PLIC_H__
#define __RISCV64_PLIC_H__

#include <rt_interrupt.h>

#define C906_PLIC_PHY_ADDR              (0x10000000)

#define PLIC_PRIO_REG(n)   (0x0000 + n*0x0004)     //PLIC Priority Register n (0 < n < 256)
#define PLIC_IP_REG(n)     (0x1000 + n*0x0004)     //PLIC Interrupt Pending Register n (0<=n<9)
#define PLIC_MIE_REG(n)    (0x2000 + n*0x0004)     //PLIC Machine Mode Interrupt Enable Register (0<=n<9)
#define PLIC_SIE_REG(n)    (0x2080 + n*0x0004)     //PLIC Superuser Mode Interrupt Enable Register (0<=n<9)
#define PLIC_CTRL_REG      (0x1FFFFC)              //PLIC Control Register
#define PLIC_MTH_REG       (0x200000)              //PLIC Machine Threshold Register
#define PLIC_MCLAIM_REG    (0x200004)              //PLIC Machine Claim Register
#define PLIC_STH_REG       (0x201000)              //PLIC Superuser Threshold Register
#define PLIC_SCLAIM_REG    (0x201004)              //PLIC Superuser Claim Register

void c906_plic_priority_set(rt_uint32_t irq_num, rt_uint32_t priority);
int c906_plic_pending_get(rt_uint32_t irq_num);
void c906_plic_mmode_enable(rt_uint32_t irq_num);
void c906_plic_ctrl(int ctrl);
void c906_plic_mmode_threshold(int th);
void c906_plic_mmode_mclaim_complete(int id);
int c906_plic_mmode_mclaim_get(void);
int c906_plic_mmode_threshold_get(void);
int c906_plic_priority_get(rt_uint32_t irq_num);

void plic_complete(int irq);
void plic_handle_irq(void);

#endif
