/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-30     lizhirui     first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "tick.h"

#include "drv_uart.h"
#include "encoding.h"
#include "stack.h"
#include "sbi.h"
#include "riscv.h"
#include "stack.h"

static void dcache_enable(void)
{
	write_csr(0x7c2, 0x70013);		/* csr_write(mcor, 0x70013); */
	write_csr(0x7c1, 0x11ff);		/* csr_write(mhcr, 0x11ff); */
	write_csr(0x7c0, 0x638000);		/* csr_set(mxstatus, 0x638000); */
	write_csr(0x7c5, 0x16e30c);		/* csr_write(mhint, 0x16e30c); */
}

void rt_hw_board_init(void)
{
    sys_clock_init();
    rt_hw_interrupt_init();
    dcache_enable();
    /* initalize interrupt */
    rt_hw_uart_init();
    
    /* initialize hardware interrupt */
    rt_hw_tick_init();
#ifdef RT_USING_HEAP
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_console_set_device("uart");
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
#ifdef RT_USING_HEAP
    rt_kprintf("heap: [0x%08x - 0x%08x]\n", (rt_ubase_t) RT_HW_HEAP_BEGIN, (rt_ubase_t) RT_HW_HEAP_END);
#endif

}
