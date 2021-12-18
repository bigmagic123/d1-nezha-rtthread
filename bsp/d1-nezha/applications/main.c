/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include <rthw.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    printf("Hello RISC-V\n");

    while (1)
    {
        rt_kprintf("cur tick 1 is %d\n", rt_tick_get());
        rt_thread_delay(1);
        rt_kprintf("cur tick 2 is %d\n", rt_tick_get());
        rt_thread_delay(5);
        rt_kprintf("cur tick  3 is %d\n", rt_tick_get());
    }
    return 0;
}
