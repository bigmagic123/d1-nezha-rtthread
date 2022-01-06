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
#include <drv_gpio.h>
#include <drivers/pin.h>

int main(void)
{
    printf("Hello RISC-V!\n");

    int led_pin = rt_pin_get("PD.22");
    rt_pin_mode(led_pin, PIN_MODE_OUTPUT);

    while(1)
    {
        rt_pin_write(led_pin, 1);
        rt_thread_mdelay(500);
        rt_pin_write(led_pin, 0);
        rt_thread_mdelay(500);
    }
    return 0;
}
