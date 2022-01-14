#ifndef __DRV_WDT_H__
#define __DRV_WDT_H__

#define KEY_FIELD       (0x16AA)
#define WDOG_KEY_FIELD  (0xA57)

//init watchdog
#define WDOG_IRQ_EN_REG         (0x020500A0)
#define WDOG_IRQ_STA_REG        (0x020500A4)
#define WDOG_SOFT_RST_REG       (0x020500A8)
#define WDOG_CTRL_REG           (0x020500B0)
#define WDOG_CFG_REG            (0x020500B4)
#define WDOG_MODE_REG           (0x020500B8)
#define WDOG_OUTPUT_CFG_REG     (0x020500BC)

#define IRQ_WATCHDOG_NUM        (79)
#endif