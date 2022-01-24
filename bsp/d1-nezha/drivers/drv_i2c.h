#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#define TWI0_ADDR_BASE (0x02502000)
#define TWI1_ADDR_BASE (0x02502400)
#define TWI2_ADDR_BASE (0x02502800)
#define TWI3_ADDR_BASE (0x02502C00)

#define TWI_ADDR_OFFSET     (0x0000)
#define TWI_XADDR_OFFSET    (0x0004)
#define TWI_DATA_OFFSET     (0x0008)
#define TWI_CNTR_OFFSET     (0x000c)
#define TWI_STAT_OFFSET     (0x0010)
#define TWI_CCR_OFFSET      (0x0014)
#define TWI_SRST_OFFSET     (0x0018)
#define TWI_EFR_OFFSET      (0x001c)
#define TWI_LCR_OFFSET      (0x0020)

#define TWI_DRV_CTRL_OFFSET     (0x0200)
#define TWI_DRV_CFG_OFFSET      (0x0204)
#define TWI_DRV_SLV_OFFSET      (0x0208)
#define TWI_DRV_FMT_OFFSET      (0x020C)
#define TWI_DRV_BUS_CTRL        (0x0210)
#define TWI_DRV_INT_CTRL        (0x0214)
#define TWI_DRV_DMA_CFG         (0x0218)
#define TWI_DRV_FIFO_CON        (0x021C)
#define TWI_DRV_SEND_FIFO_ACC   (0x0300)
#define TWI_DRV_RECV_FIFO_ACC   (0x0304)

#endif