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
        rt_kprintf("cur tick is %d\n", rt_tick_get());
        rt_thread_mdelay(1000);
    }
    return 0;
}
