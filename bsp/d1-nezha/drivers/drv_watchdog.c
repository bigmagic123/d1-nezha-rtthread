#include <drv_watchdog.h>
#include <drivers/watchdog.h>
#include <common.h>

rt_uint32_t wdt_timer_set = 0;

void watchdog_irq_enable(void)
{
    write32(WDOG_IRQ_EN_REG, 0x01);
}

//read and clear
int watchdog_get_status(void)
{
    int val = read32(WDOG_IRQ_STA_REG);
    write32(WDOG_IRQ_STA_REG, val);
    return val;
}

//reset watchdog , watchdog first needs to be disabled
void watchdog_reset_system(void)
{
    write32(WDOG_SOFT_RST_REG, (KEY_FIELD << 16) | (0x01));//soft reset enable
}

//watchdog config
void watchdog_ctrl_reset(void)
{
    write32(WDOG_CTRL_REG, (WDOG_KEY_FIELD << 1) | (0x01));//reset watchdog
}

void watchdog_cfg(int irq_enable, int clk)
{
    if(irq_enable)
    {
        write32(WDOG_CFG_REG, (KEY_FIELD << 16) | (clk << 8) | (0x10));
    }
    else
    {
        write32(WDOG_CFG_REG, (KEY_FIELD << 16) | (clk << 8) | (0x01));
    }
    
}

void watchdog_mode_set(int interval_val)
{
    write32(WDOG_MODE_REG, (KEY_FIELD << 16) | (interval_val << 4) | (0x01));//0:0.5s 1 1s
}

void watchdog_set_reset_time(int val)
{
    // 1/32ms * (N+1), default 1F (1ms)
    write32(WDOG_OUTPUT_CFG_REG, val);
}

void watchdog_stop()
{
    write32(WDOG_MODE_REG, (KEY_FIELD << 16) | (wdt_timer_set << 4) | (0x00));
}

static rt_err_t d1s_wdg_init(rt_watchdog_t *wdt)
{
    return RT_EOK;
}

static rt_err_t d1s_wdg_control(rt_watchdog_t *wdt, int cmd, void *arg)
{
    rt_uint64_t  timeout_us = 0;
    switch (cmd)
    {
    case RT_DEVICE_CTRL_WDT_SET_TIMEOUT:
        wdt_timer_set = ((rt_uint32_t *)arg);
        break;
    case RT_DEVICE_CTRL_WDT_GET_TIMEOUT:
        *((rt_uint32_t *)arg) = wdt_timer_set;
        break;
    case RT_DEVICE_CTRL_WDT_GET_TIMELEFT:
        break;
    case RT_DEVICE_CTRL_WDT_KEEPALIVE:
        watchdog_ctrl_reset();
        break;
    case RT_DEVICE_CTRL_WDT_START:
        watchdog_mode_set(wdt_timer_set);
        break;
    case RT_DEVICE_CTRL_WDT_STOP:
        watchdog_stop();
        break;
    default:
        return RT_EIO;
    }
    return RT_EOK;
}

static const struct rt_watchdog_ops d1s_wdg_pos =
{
    d1s_wdg_init,
    d1s_wdg_control,
};

int rt_hw_wdt_init(void)
{
    static rt_watchdog_t d1s_wdg;

    d1s_wdg.ops = &d1s_wdg_pos;

    rt_hw_watchdog_register(&d1s_wdg, "wdg", 0, RT_NULL);

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_wdt_init);

int reboot(void)
{
    unsigned int r;

    rt_kprintf("reboot system...\n");
    rt_thread_mdelay(100);
    watchdog_ctrl_reset();
    watchdog_cfg(0, 1);
    watchdog_mode_set(1);

    while (1);
    return 0;
}
MSH_CMD_EXPORT(reboot,reboot system...);