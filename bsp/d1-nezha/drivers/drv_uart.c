/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <rthw.h>
#include <rtdevice.h>

#include "board.h"
#include "drv_uart.h"

#include <stdio.h>
#include "sbi.h"

#define UART_DEFAULT_BAUDRATE               115200
#define SUNXI_UART_ADDR            0x02500000

#define SUNXI_UART_RBR (0x00)       /* receive buffer register */
#define SUNXI_UART_THR (0x00)       /* transmit holding register */
#define SUNXI_UART_DLL (0x00)       /* divisor latch low register */
#define SUNXI_UART_DLH (0x04)       /* diviso latch high register */
#define SUNXI_UART_IER (0x04)       /* interrupt enable register */
#define SUNXI_UART_IIR (0x08)       /* interrupt identity register */
#define SUNXI_UART_FCR (0x08)       /* FIFO control register */
#define SUNXI_UART_LCR (0x0c)       /* line control register */
#define SUNXI_UART_MCR (0x10)       /* modem control register */
#define SUNXI_UART_LSR (0x14)       /* line status register */
#define SUNXI_UART_MSR (0x18)       /* modem status register */
#define SUNXI_UART_SCH (0x1c)       /* scratch register */
#define SUNXI_UART_USR (0x7c)       /* status register */
#define SUNXI_UART_TFL (0x80)       /* transmit FIFO level */
#define SUNXI_UART_RFL (0x84)       /* RFL */
#define SUNXI_UART_HALT (0xa4)      /* halt tx register */
#define SUNXI_UART_RS485 (0xc0)     /* RS485 control and status register */

#define BIT(x) (1 << x)

/* Line Status Rigster */
#define SUNXI_UART_LSR_RXFIFOE    (BIT(7))
#define SUNXI_UART_LSR_TEMT       (BIT(6))
#define SUNXI_UART_LSR_THRE       (BIT(5))
#define SUNXI_UART_LSR_BI         (BIT(4))
#define SUNXI_UART_LSR_FE         (BIT(3))
#define SUNXI_UART_LSR_PE         (BIT(2))
#define SUNXI_UART_LSR_OE         (BIT(1))
#define SUNXI_UART_LSR_DR         (BIT(0))
#define SUNXI_UART_LSR_BRK_ERROR_BITS 0x1E /* BI, FE, PE, OE bits */

struct device_uart
{
    rt_ubase_t  hw_base;
    rt_uint32_t irqno;
};

static rt_err_t  rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg);
static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg);
static int       drv_uart_putc(struct rt_serial_device *serial, char c);
static int       drv_uart_getc(struct rt_serial_device *serial);

const struct rt_uart_ops _uart_ops =
{
    rt_uart_configure,
    uart_control,
    drv_uart_putc,
    drv_uart_getc,
    //TODO: add DMA support
    RT_NULL
};

void uart_init(void)
{
    return ;
}

struct rt_serial_device  serial1;
struct device_uart       uart1;

/*
 * UART interface
 */
static rt_err_t rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct device_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    serial->config = *cfg;

    return (RT_EOK);
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct device_uart *uart;

    uart = serial->parent.user_data;
    rt_uint32_t channel = 1;

    RT_ASSERT(uart != RT_NULL);
    RT_ASSERT(channel != 3);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        break;

    case RT_DEVICE_CTRL_SET_INT:
        break;
    }

    return (RT_EOK);
}

static int drv_uart_putc(struct rt_serial_device *serial, char c)
{
    sbi_console_putchar(c);
    return (1);
}

static int drv_uart_getc(struct rt_serial_device *serial)
{
    uint32_t *lsr = (uint32_t *)(SUNXI_UART_ADDR + SUNXI_UART_LSR);
    uint32_t *rbr = (uint32_t *)(SUNXI_UART_ADDR + SUNXI_UART_RBR);

    if(!(*lsr & SUNXI_UART_LSR_DR))
    {
        return -1;
    }

    return (int)*rbr;
}

//串口接收监视线程
static void uart_rx(void *param)
{
    struct rt_serial_device *serial = (struct rt_serial_device *)param;

    while(1)
    {
        rt_hw_serial_isr((struct rt_serial_device *)serial,RT_SERIAL_EVENT_RX_IND);
        rt_thread_mdelay(10);
    }
}

//用于启动串口接收监视线程
void rt_hw_uart_start_rx_thread()
{
    rt_thread_t th;
    RT_ASSERT((th = rt_thread_create("uartrx",uart_rx,(void *)&serial1,8192,8,20)) != RT_NULL);
    RT_ASSERT(rt_thread_startup(th) == RT_EOK);
}

/*
 * UART Initiation
 */
int rt_hw_uart_init(void)
{
    struct rt_serial_device *serial;
    struct device_uart      *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    {
        serial  = &serial1;
        uart    = &uart1;

        serial->ops              = &_uart_ops;
        serial->config           = config;
        serial->config.baud_rate = UART_DEFAULT_BAUDRATE;

        uart->hw_base   = 0x10000000;
        uart->irqno     = 0xa;

        rt_hw_serial_register(serial,
                              "uart",
                              RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              uart);
    }

    return 0;
}

/* WEAK for SDK 0.5.6 */
RT_WEAK void uart_debug_init(int uart_channel)
{
}
