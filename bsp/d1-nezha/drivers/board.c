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

#define D1_GPIO_BASE    (0x02000000)

//PB 13 (0~12)
#define D1_GPIO_PB_CFG0  (0x0030)
#define D1_GPIO_PB_CFG1  (0x0034)
#define D1_GPIO_PB_DAT   (0x0040)
#define D1_GPIO_PB_DRV0  (0x0044)
#define D1_GPIO_PB_DRV1  (0x0048)
#define D1_GPIO_PB_PULL0 (0x0054)

//PC 8 (0~7)
#define D1_GPIO_PC_CFG0  (0x0060)
#define D1_GPIO_PC_DAT   (0x0070)
#define D1_GPIO_PC_DRV0  (0x0074)
#define D1_GPIO_PC_PULL0 (0x0084)

//PD 23 (0~22)
#define D1_GPIO_PD_CFG0  (0x0090)
#define D1_GPIO_PD_CFG1  (0x0094)
#define D1_GPIO_PD_CFG2  (0x0098)
#define D1_GPIO_PD_DAT   (0x00A0)
#define D1_GPIO_PD_DRV0  (0x00A4)
#define D1_GPIO_PD_DRV1  (0x00A8)
#define D1_GPIO_PD_DRV2  (0x00AC)
#define D1_GPIO_PD_PULL0 (0x00B4)
#define D1_GPIO_PD_PULL1 (0x00B8)

//PE 18 (0~17)
#define D1_GPIO_PE_CFG0  (0x00C0)
#define D1_GPIO_PE_CFG1  (0x00C4)
#define D1_GPIO_PE_DAT   (0x00D0)
#define D1_GPIO_PE_DRV0  (0x00D4)
#define D1_GPIO_PE_DRV1  (0x00D8)
#define D1_GPIO_PE_PULL0 (0x00E4)

//PF 7  (0~7)
#define D1_GPIO_PF_CFG0  (0x00F0)
#define D1_GPIO_PF_DAT   (0x0100)
#define D1_GPIO_PF_DRV0  (0x0104)
#define D1_GPIO_PF_PULL0 (0x0114)

//PG 19 (0~18)
#define D1_GPIO_PG_CFG0  (0x0120)
#define D1_GPIO_PG_CFG1  (0x0124)
#define D1_GPIO_PG_DAT   (0x0130)
#define D1_GPIO_PG_DRV0  (0x0134)
#define D1_GPIO_PG_DRV1  (0x0138)
#define D1_GPIO_PG_DRV3  (0x0140)
#define D1_GPIO_PG_PULL0 (0x0124)

//EINT
//PB
#define D1_GPIO_PB_EINT_CFG0    (0x0220)
#define D1_GPIO_PB_EINT_CTL     (0x0230)
#define D1_GPIO_PB_EINT_STATUS  (0x0234)
#define D1_GPIO_PB_EINT_DEB     (0x0238)

//PC
#define D1_GPIO_PC_EINT_CFG0    (0x0240)
#define D1_GPIO_PC_EINT_CTL     (0x0250)
#define D1_GPIO_PC_EINT_STATUS  (0x0254)
#define D1_GPIO_PC_EINT_DEB     (0x0258)

//PD
#define D1_GPIO_PD_EINT_CFG0    (0x0260)
#define D1_GPIO_PD_EINT_CFG1    (0x0264)
#define D1_GPIO_PD_EINT_CFG2    (0x0268)
#define D1_GPIO_PD_EINT_CTL     (0x0270)
#define D1_GPIO_PD_EINT_STATUS  (0x0274)
#define D1_GPIO_PD_EINT_DEB     (0x0278)

//PE
#define D1_GPIO_PE_EINT_CFG0    (0x0280)
#define D1_GPIO_PE_EINT_CFG1    (0x0284)
#define D1_GPIO_PE_EINT_CTL     (0x0290)
#define D1_GPIO_PE_EINT_STATUS  (0x0294)
#define D1_GPIO_PE_EINT_DEB     (0x0298)

//PF
#define D1_GPIO_PF_EINT_CFG0    (0x02A0)
#define D1_GPIO_PF_EINT_CTL     (0x02B0)
#define D1_GPIO_PF_EINT_STATUS  (0x02B4)
#define D1_GPIO_PF_EINT_DEB     (0x02B8)

//PG
#define D1_GPIO_PG_EINT_CFG0    (0x02C0)
#define D1_GPIO_PG_EINT_CFG1    (0x02C4)
#define D1_GPIO_PG_EINT_CTL     (0x02D0)
#define D1_GPIO_PG_EINT_STATUS  (0x02D4)
#define D1_GPIO_PG_EINT_DEB     (0x02D8)

#define PIO_POW_MOD_SEL         (0x0340)
#define PIO_POW_MS_CTL          (0x0344)
#define PIO_POW_VAL             (0x0348)
#define PIO_POW_VOL_SET_CTL     (0x0350)

#define     GPIO_PORT_B       (D1_GPIO_BASE + D1_GPIO_PB_CFG0)
#define     GPIO_PORT_C       (D1_GPIO_BASE + D1_GPIO_PC_CFG0)
#define     GPIO_PORT_D       (D1_GPIO_BASE + D1_GPIO_PD_CFG0)
#define     GPIO_PORT_E       (D1_GPIO_BASE + D1_GPIO_PE_CFG0)
#define     GPIO_PORT_F       (D1_GPIO_BASE + D1_GPIO_PF_CFG0)
#define     GPIO_PORT_G       (D1_GPIO_BASE + D1_GPIO_PG_CFG0)

#define     GPIO_PIN_0        (0)
#define     GPIO_PIN_1        (1)
#define     GPIO_PIN_2        (2)
#define     GPIO_PIN_3        (3)
#define     GPIO_PIN_4        (4)
#define     GPIO_PIN_5        (5)
#define     GPIO_PIN_6        (6)
#define     GPIO_PIN_7        (7)
#define     GPIO_PIN_8        (8)
#define     GPIO_PIN_9        (9)
#define     GPIO_PIN_10       (10)
#define     GPIO_PIN_11       (11)
#define     GPIO_PIN_12       (12)
#define     GPIO_PIN_13       (13)
#define     GPIO_PIN_14       (14)
#define     GPIO_PIN_15       (15)
#define     GPIO_PIN_16       (16)
#define     GPIO_PIN_17       (17)
#define     GPIO_PIN_18       (18)
#define     GPIO_PIN_19       (19)
#define     GPIO_PIN_20       (20)
#define     GPIO_PIN_21       (21)
#define     GPIO_PIN_22       (22)
#define     GPIO_PIN_23       (23)
#define     GPIO_PIN_24       (24)
#define     GPIO_PIN_25       (25)

#define D1_CCU_BASE                        (0x02001000)    //D1 CCU

#define CCU_PLL_CPU_CTRL_REG               (0x000)
#define CCU_PLL_DDR_CTRL_REG               (0x010)
#define CCU_PLL_PERI0_CTRL_REG             (0x020)
#define CCU_PLL_PERI1_CTRL_REG             (0x028)         //NO USER
#define CCU_PLL_GPU_CTRL_REG               (0x030)         //NO USER
#define CCU_PLL_VIDEO0_CTRL_REG            (0x040)
#define CCU_PLL_VIDEO1_CTRL_REG            (0x048)
#define CCU_PLL_VE_CTRL                    (0x058)
#define CCU_PLL_DE_CTRL                    (0x060)
#define CCU_PLL_HSIC_CTRL                  (0x070)
#define CCU_PLL_AUDIO0_CTRL_REG            (0x078)
#define CCU_PLL_AUDIO1_CTRL_REG            (0x080)
#define CCU_PLL_DDR_PAT0_CTRL_REG          (0x110)
#define CCU_PLL_DDR_PAT1_CTRL_REG          (0x114)
#define CCU_PLL_PERI0_PAT0_CTRL_REG        (0x120)
#define CCU_PLL_PERI0_PAT1_CTRL_REG        (0x124)
#define CCU_PLL_PERI1_PAT0_CTRL_REG        (0x128)
#define CCU_PLL_PERI1_PAT1_CTRL_REG        (0x12c)
#define CCU_PLL_GPU_PAT0_CTRL_REG          (0x130)
#define CCU_PLL_GPU_PAT1_CTRL_REG          (0x134)
#define CCU_PLL_VIDEO0_PAT0_CTRL_REG       (0x140)
#define CCU_PLL_VIDEO0_PAT1_CTRL_REG       (0x144)
#define CCU_PLL_VIDEO1_PAT0_CTRL_REG       (0x148)
#define CCU_PLL_VIDEO1_PAT1_CTRL_REG       (0x14c)
#define CCU_PLL_VE_PAT0_CTRL_REG           (0x158)
#define CCU_PLL_VE_PAT1_CTRL_REG           (0x15c)
#define CCU_PLL_DE_PAT0_CTRL_REG           (0x160)
#define CCU_PLL_DE_PAT1_CTRL_REG           (0x164)
#define CCU_PLL_HSIC_PAT0_CTRL_REG         (0x170)
#define CCU_PLL_HSIC_PAT1_CTRL_REG         (0x174)
#define CCU_PLL_AUDIO0_PAT0_CTRL_REG       (0x178)
#define CCU_PLL_AUDIO0_PAT1_CTRL_REG       (0x17c)
#define CCU_PLL_AUDIO1_PAT0_CTRL_REG       (0x180)
#define CCU_PLL_AUDIO1_PAT1_CTRL_REG       (0x184)
#define CCU_PLL_CPU_BIAS_REG               (0x300)
#define CCU_PLL_DDR_BIAS_REG               (0x310)
#define CCU_PLL_PERI0_BIAS_REG             (0x320)
#define CCU_PLL_PERI1_BIAS_REG             (0x328)
#define CCU_PLL_GPU_BIAS_REG               (0x330)
#define CCU_PLL_VIDEO0_BIAS_REG            (0x340)
#define CCU_PLL_VIDEO1_BIAS_REG            (0x348)
#define CCU_PLL_VE_BIAS_REG                (0x358)
#define CCU_PLL_DE_BIAS_REG                (0x360)
#define CCU_PLL_HSIC_BIAS_REG              (0x370)
#define CCU_PLL_AUDIO0_BIAS_REG            (0x378)
#define CCU_PLL_AUDIO1_BIAS_REG            (0x380)
#define CCU_PLL_CPU_TUN_REG                (0x400)
#define CCU_CPU_AXI_CFG_REG                (0x500)
#define CCU_CPU_GATING_REG                 (0x504)
#define CCU_PSI_CLK_REG                    (0x510)
#define CCU_AHB3_CLK_REG                   (0x51c)
#define CCU_APB0_CLK_REG                   (0x520)
#define CCU_APB1_CLK_REG                   (0x524)
#define CCU_MBUS_CLK_REG                   (0x540)
#define CCU_DMA_BGR_REG                    (0x70c)
#define CCU_DRAM_CLK_REG                   (0x800)
#define CCU_MBUS_MAT_CLK_GATING_REG        (0x804)
#define CCU_DRAM_BGR_REG                   (0x80c)
#define CCU_UART_BGR_REG                   (0x90C)
#define CCU_TWI_BGR_REG                    (0x91C)
#define CCU_SPI0_BGR_REG                   (0x940)
#define CCU_SPI1_BGR_REG                   (0x944)
#define CCU_SPI_BGR_REG                    (0x96C)
#define CCU_RISCV_CLK_REG                  (0xd00)
#define CCU_RISCV_GATING_REG               (0xd04)
#define CCU_RISCV_CFG_BGR_REG              (0xd0c)

typedef unsigned int virtual_addr_t;
typedef unsigned int u32_t;

typedef unsigned long uint64_t;
typedef unsigned int uint32_t;

static inline void write32(virtual_addr_t addr, u32_t value)
{
    *((volatile u32_t *)(addr)) = value;
}

static inline u32_t read32(virtual_addr_t addr)
{
    return( *((volatile u32_t *)(addr)));
}

#ifdef RT_USING_USERSPACE
    #include "riscv_mmu.h"
    #include "mmu.h"
    #include "page.h"
    #include "lwp_arch.h"

    //这个结构体描述了buddy system的页分配范围
    rt_region_t init_page_region =
    {
        (rt_size_t)RT_HW_PAGE_START,
        (rt_size_t)RT_HW_PAGE_END
    };

    //内核页表
    volatile rt_size_t MMUTable[__SIZE(VPN2_BIT)] __attribute__((aligned(4 * 1024)));
    rt_mmu_info mmu_info;

#endif

//初始化BSS节区
void init_bss(void)
{
    unsigned int *dst;

    dst = &__bss_start;
    while (dst < &__bss_end)
    {
        *dst++ = 0;
    }
}

static void __rt_assert_handler(const char *ex_string, const char *func, rt_size_t line)
{
    rt_kprintf("(%s) assertion failed at function:%s, line number:%d \n", ex_string, func, line);
    asm volatile("ebreak":::"memory");
}





//D1 debug uart use GPIOB8(TX0) and GPIOB9(RX0)
#define UART_BASE   (0X02500000)
#define UART1_BASE  (0X02500400)
#define UART2_BASE  (0X02500800)
#define UART3_BASE  (0X02500C00)
#define UART4_BASE  (0X02501000)
#define UART5_BASE  (0X02501400)


#define UART_RBR            (0x0000)
#define UART_THR            (0x0000)
#define UART_DLL            (0x0000)
#define UART_DLH            (0x0004)
#define UART_IER            (0x0004)
#define UART_IIR            (0x0008)
#define UART_FCR            (0x0008)
#define UART_LCR            (0x000C)
#define UART_MCR            (0x0010)
#define UART_LSR            (0x0014)
#define UART_MSR            (0x0018)
#define UART_SCH            (0x001C)
#define UART_USR            (0x007C)
#define UART_TFL            (0x0080)
#define UART_RFL            (0x0084)
#define UART_HSK            (0x0088)
#define UART_DMA_REQ_EN     (0x008C)
#define UART_HALT           (0x00A4)
#define UART_DBG_DLL        (0x00B0)
#define UART_DBG_DLH        (0x00B4)
#define UART_A_FCC          (0x00F0)
#define UART_A_RXDMA_CTRL   (0x0100)
#define UART_A_RXDMA_STR    (0x0104)
#define UART_A_RXDMA_STA    (0x0108)
#define UART_A_RXDMA_LMT    (0x010C)
#define UART_A_RXDMA_SADDRL           (0x0110)
#define UART_A_RXDMA_SADDRH           (0x0114)
#define UART_A_RXDMA_BL     (0x0118)
#define UART_A_RXDMA_IE     (0x0120)
#define UART_A_RXDMA_IS     (0x0124)
#define UART_A_RXDMA_WADDRL           (0x0128)
#define UART_A_RXDMA_WADDRH           (0x012C)
#define UART_A_RXDMA_RADDRL           (0x0130)
#define UART_A_RXDMA_RADDRH           (0x0134)
#define UART_A_RXDMA_DCNT   (0x0138)

#define UART_LSR_FIFOE		0x80    /* Fifo error */
#define UART_LSR_TEMT		0x40    /* Transmitter empty */
#define UART_LSR_THRE		0x20    /* Transmit-hold-register empty */
#define UART_LSR_BI		0x10    /* Break interrupt indicator */
#define UART_LSR_FE		0x08    /* Frame error indicator */
#define UART_LSR_PE		0x04    /* Parity error indicator */
#define UART_LSR_OE		0x02    /* Overrun error indicator */
#define UART_LSR_DR		0x01    /* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS	0x1E    /* BI, FE, PE, OE bits */



void d1_set_gpio_mode(uint32_t gpio_port, uint32_t gpio_pin, uint16_t mode)
{
    uint32_t pin_level = 0;
    uint32_t gpio_base_addr = 0;
    uint32_t val = 0;
    pin_level = gpio_pin / 8;
    gpio_base_addr = gpio_port + pin_level * 0x04;
    val = read32(gpio_base_addr);

    val &= ~(0xf << ((gpio_pin & 0x7) << 2));
    val |= ((mode & 0xf) << ((gpio_pin & 0x7) << 2));

    write32(gpio_base_addr, val);
}

void d1_set_gpio_val(uint32_t gpio_port, uint32_t gpio_pin, uint32_t val)
{
    
}

#define UART0_MODE_TX   (6)
#define UART0_MODE_RX   (6)

void sys_uart_putc(uint8_t uart_num, char c)
{
    virtual_addr_t addr = UART_BASE + uart_num * 0x4000;

    while((read32(addr + UART_LSR) & UART_LSR_THRE) == 0);
    write32(addr + UART_THR, c);
}

char sys_uart_getc(uint8_t uart_num)
{
    virtual_addr_t addr = UART_BASE + uart_num * 0x4000;
    if((read32(addr + UART_LSR) & UART_LSR_DR))
    {
        return read32(addr + UART_RBR);
    }
    else
    {
        return -1;
    }
}

void sys_uart0_init(void)
{
    virtual_addr_t addr;
    u32_t val;

    d1_set_gpio_mode(GPIO_PORT_B, GPIO_PIN_8, UART0_MODE_TX);
    d1_set_gpio_mode(GPIO_PORT_B, GPIO_PIN_9, UART0_MODE_RX);

    clk_enable_module_uart(D1_CCU_BASE + CCU_UART_BGR_REG, 0);

    /* Config uart0 to 115200-8-1-0 */
    addr = UART_BASE + 0 * 0x4000;
    write32(addr + UART_DLH, 0x0);      //disable all interrupt
    write32(addr + UART_FCR, 0xf7);     //reset fifo
    write32(addr + UART_MCR, 0x0);      //uart mode
    //set 115200
    val = read32(addr + UART_LCR);
    val |= (1 << 7);                    //select Divisor Latch LS Register
    write32(addr + UART_LCR, val);
    write32(addr + UART_DLL, 0xd & 0xff);   // 0x0d=13 240000000/(13*16) = 115200 Divisor Latch Lows
    write32(addr + UART_DLH, (0xd >> 8) & 0xff); //Divisor Latch High
    val = read32(addr + UART_LCR);
    val &= ~(1 << 7);
    write32(addr + UART_LCR, val);

    val = read32(addr + UART_LCR);
    val &= ~0x1f;
    val |= (0x3 << 0) | (0 << 2) | (0x0 << 3); //8 bit, 1 stop bit,parity disabled
    write32(addr + UART_LCR, val);
}

static inline uint64_t counter(void)
{
    uint64_t cnt;
    __asm__ __volatile__("csrr %0, time\n" : "=r"(cnt) :: "memory");
    return cnt;
}

void sdelay(unsigned long us)
{
    uint64_t t1 = counter();
    uint64_t t2 = t1 + us * 24;
    do {
        t1 = counter();
    } while(t2 >= t1);
}




static void set_pll_cpux_axi(void)
{
    uint32_t val;

    /* Select cpux clock src to osc24m, axi divide ratio is 3, system apb clk ratio is 4 */
    write32(D1_CCU_BASE + CCU_RISCV_CLK_REG, (0 << 24) | (3 << 8) | (1 << 0));
    sdelay(1);

    /* Disable pll gating */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val &= ~(1 << 27);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

    /* Enable pll ldo */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val |= (1 << 30);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);
    sdelay(5);

    /* Set default clk to 1008mhz */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val &= ~((0x3 << 16) | (0xff << 8) | (0x3 << 0));
    val |= (41 << 8);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

    /* Lock enable */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val |= (1 << 29);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

    /* Enable pll */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val |= (1 << 31);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

    /* Wait pll stable */
    while(!(read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG) & (0x1 << 28)));
    sdelay(20);

    /* Enable pll gating */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val |= (1 << 27);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

    /* Lock disable */
    val = read32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
    val &= ~(1 << 29);
    write32(D1_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);
    sdelay(1);

    /* Set and change cpu clk src */
    val = read32(D1_CCU_BASE + CCU_RISCV_CLK_REG);
    val &= ~(0x07 << 24 | 0x3 << 8 | 0xf << 0);
    val |= (0x05 << 24 | 0x1 << 8);
    write32(D1_CCU_BASE + CCU_RISCV_CLK_REG, val);
    sdelay(1);
}

static void set_pll_periph0(void)
{
    uint32_t val;

    /* Periph0 has been enabled */
    if(read32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG) & (1 << 31))
        return;

    /* Change psi src to osc24m */
    val = read32(D1_CCU_BASE + CCU_PSI_CLK_REG);
    val &= (~(0x3 << 24));
    write32(val, D1_CCU_BASE + CCU_PSI_CLK_REG);

    /* Set default val */
    write32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, 0x63 << 8);

    /* Lock enable */
    val = read32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
    val |= (1 << 29);
    write32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, val);

    /* Enabe pll 600m(1x) 1200m(2x) */
    val = read32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
    val |= (1 << 31);
    write32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, val);

    /* Wait pll stable */
    while(!(read32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG) & (0x1 << 28)));
    sdelay(20);

    /* Lock disable */
    val = read32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
    val &= ~(1 << 29);
    write32(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, val);
}

static void set_ahb(void)
{
    write32(D1_CCU_BASE + CCU_PSI_CLK_REG, (2 << 0) | (0 << 8));
    write32(D1_CCU_BASE + CCU_PSI_CLK_REG, read32(D1_CCU_BASE + CCU_PSI_CLK_REG) | (0x03 << 24));
    sdelay(1);
}

static void set_apb(void)
{
    write32(D1_CCU_BASE + CCU_APB0_CLK_REG, (2 << 0) | (1 << 8));
    write32(D1_CCU_BASE + CCU_APB0_CLK_REG, (0x03 << 24) | read32(D1_CCU_BASE + CCU_APB0_CLK_REG));
    sdelay(1);
}

static void set_dma(void)
{
    /* Dma reset */
    write32(D1_CCU_BASE + CCU_DMA_BGR_REG, read32(D1_CCU_BASE + CCU_DMA_BGR_REG) | (1 << 16));
    sdelay(20);
    /* Enable gating clock for dma */
    write32(D1_CCU_BASE + CCU_DMA_BGR_REG, read32(D1_CCU_BASE + CCU_DMA_BGR_REG) | (1 << 0));
}

static void set_mbus(void)
{
    uint32_t val;

    /* Reset mbus domain */
    val = read32(D1_CCU_BASE + CCU_MBUS_CLK_REG);
    val |= (0x1 << 30);
    write32(D1_CCU_BASE + CCU_MBUS_CLK_REG, val);
    sdelay(1);
}

static void set_module(virtual_addr_t addr)
{
    uint32_t val;

    if(!(read32(addr) & (1 << 31)))
    {
        val = read32(addr);
        write32(addr, val | (1 << 31) | (1 << 30));

        /* Lock enable */
        val = read32(addr);
        val |= (1 << 29);
        write32(addr, val);

        /* Wait pll stable */
        while(!(read32(addr) & (0x1 << 28)));
        sdelay(20);

        /* Lock disable */
        val = read32(addr);
        val &= ~(1 << 29);
        write32(addr, val);
    }
}

void clk_enable_module_uart(virtual_addr_t addr, uint8_t uart_num)
{
    uint32_t val;
    /* Open the clock gate for uart */
    val = read32(addr);
    val |= 1 << (0 + uart_num);
    write32(addr, val);

    /* Deassert uart reset */
    val = read32(addr);
    val |= 1 << (16 + uart_num);
    write32(addr, val);
}

void sys_clock_init(void)
{
    set_pll_cpux_axi();
    set_pll_periph0();
    set_ahb();
    set_apb();
    set_dma();
    set_mbus();
    set_module(D1_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
    set_module(D1_CCU_BASE + CCU_PLL_VIDEO0_CTRL_REG);
    set_module(D1_CCU_BASE + CCU_PLL_VIDEO1_CTRL_REG);
    set_module(D1_CCU_BASE + CCU_PLL_VE_CTRL);
    set_module(D1_CCU_BASE + CCU_PLL_AUDIO0_CTRL_REG);
    set_module(D1_CCU_BASE + CCU_PLL_AUDIO1_CTRL_REG);
}

//BSP的C入口
void primary_cpu_entry(void)
{
    extern void entry(void);

    //初始化BSS
    init_bss();
    //关中断
    rt_hw_interrupt_disable();
    rt_assert_set_hook(__rt_assert_handler);
    //启动RT-Thread Smart内核

    sys_clock_init();
    sys_uart0_init();

    entry();
}


//这个初始化程序由内核主动调用，此时调度器还未启动，因此在此不能使用依赖线程上下文的函数
void rt_hw_board_init(void)
{
    rt_hw_interrupt_init();
    /* initalize interrupt */
    rt_hw_uart_init();
    
    /* initialize hardware interrupt */
    //rt_hw_tick_init();
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

#ifdef RT_USING_USERSPACE
    rt_page_init(init_page_region);
    rt_hw_mmu_map_init(&mmu_info,(void *)USER_VADDR_START, USER_VADDR_TOP - USER_VADDR_START, (rt_size_t *)MMUTable, 0);
    rt_hw_mmu_kernel_map_init(&mmu_info, 0x00000000UL, USER_VADDR_START - 1);
   
    //将低1GB MMIO区域设置为无Cache与Strong Order访存模式
    MMUTable[0] &= ~PTE_CACHE;
    MMUTable[0] &= ~PTE_SHARE;
    MMUTable[0] |= PTE_SO;
    switch_mmu((void *)MMUTable);
#endif
}

void rt_hw_cpu_reset(void)
{
    sbi_shutdown();
    while(1);
}
MSH_CMD_EXPORT_ALIAS(rt_hw_cpu_reset, reboot, reset machine);
