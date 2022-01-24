#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <string.h>
#include <drv_sdio.h>
#include <common.h>
#include <drv_gpio.h>
#include <rtdevice.h>
#include <drivers/mmcsd_core.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME  "[MMC]"
/*
 * The possible values of 'DBG_LEVEL' are:
 * DBG_LOG, DBG_INFO, DBG_WARNING, DBG_ERROR
 */
#define DBG_LEVEL DBG_WARNING
#define DBG_COLOR
#include <rtdbg.h>

#define CONFIG_MMC_USE_DMA
#define DMA_ALIGN       (32)


enum {
    SD_GCTL            = 0x00,
    SD_CKCR            = 0x04,
    SD_TMOR            = 0x08,
    SD_BWDR            = 0x0c,
    SD_BKSR            = 0x10,
    SD_BYCR            = 0x14,
    SD_CMDR            = 0x18,
    SD_CAGR            = 0x1c,
    SD_RESP0        = 0x20,
    SD_RESP1        = 0x24,
    SD_RESP2        = 0x28,
    SD_RESP3        = 0x2c,
    SD_IMKR            = 0x30,
    SD_MISR            = 0x34,
    SD_RISR            = 0x38,
    SD_STAR            = 0x3c,
    SD_FWLR            = 0x40,
    SD_FUNS            = 0x44,
    SD_A12A            = 0x58,
    SD_NTSR            = 0x5c,
    SD_SDBG            = 0x60,
    SD_HWRST        = 0x78,
    SD_DMAC            = 0x80,
    SD_DLBA            = 0x84,
    SD_IDST            = 0x88,
    SD_IDIE            = 0x8c,
    SD_THLDC        = 0x100,
    SD_DSBD            = 0x10c,
    SD_RES_CRC        = 0x110,
    SD_DATA7_CRC    = 0x114,
    SD_DATA6_CRC    = 0x118,
    SD_DATA5_CRC    = 0x11c,
    SD_DATA4_CRC    = 0x120,
    SD_DATA3_CRC    = 0x124,
    SD_DATA2_CRC    = 0x128,
    SD_DATA1_CRC    = 0x12c,
    SD_DATA0_CRC    = 0x130,
    SD_CRC_STA        = 0x134,
    SD_FIFO            = 0x200,
};

/*
 * Global control register bits
 */
#define SDXC_SOFT_RESET                (1 << 0)
#define SDXC_FIFO_RESET                (1 << 1)
#define SDXC_DMA_RESET                (1 << 2)
#define SDXC_INTERRUPT_ENABLE_BIT    (1 << 4)
#define SDXC_DMA_ENABLE_BIT            (1 << 5)
#define SDXC_DEBOUNCE_ENABLE_BIT    (1 << 8)
#define SDXC_POSEDGE_LATCH_DATA        (1 << 9)
#define SDXC_DDR_MODE                (1 << 10)
#define SDXC_MEMORY_ACCESS_DONE        (1 << 29)
#define SDXC_ACCESS_DONE_DIRECT        (1 << 30)
#define SDXC_ACCESS_BY_AHB            (1 << 31)
#define SDXC_ACCESS_BY_DMA            (0 << 31)
#define SDXC_HARDWARE_RESET            (SDXC_SOFT_RESET | SDXC_FIFO_RESET | SDXC_DMA_RESET)

/*
 * Clock control bits
 */
#define SDXC_CARD_CLOCK_ON            (1 << 16)
#define SDXC_LOW_POWER_ON            (1 << 17)

/*
 * Bus width
 */
#define SDXC_WIDTH1                    (0)
#define SDXC_WIDTH4                    (1)
#define SDXC_WIDTH8                    (2)

/*
 * Smc command bits
 */
#define SDXC_RESP_EXPIRE            (1 << 6)
#define SDXC_LONG_RESPONSE            (1 << 7)
#define SDXC_CHECK_RESPONSE_CRC        (1 << 8)
#define SDXC_DATA_EXPIRE            (1 << 9)
#define SDXC_WRITE                    (1 << 10)
#define SDXC_SEQUENCE_MODE            (1 << 11)
#define SDXC_SEND_AUTO_STOP            (1 << 12)
#define SDXC_WAIT_PRE_OVER            (1 << 13)
#define SDXC_STOP_ABORT_CMD            (1 << 14)
#define SDXC_SEND_INIT_SEQUENCE        (1 << 15)
#define SDXC_UPCLK_ONLY                (1 << 21)
#define SDXC_READ_CEATA_DEV            (1 << 22)
#define SDXC_CCS_EXPIRE                (1 << 23)
#define SDXC_ENABLE_BIT_BOOT        (1 << 24)
#define SDXC_ALT_BOOT_OPTIONS        (1 << 25)
#define SDXC_BOOT_ACK_EXPIRE        (1 << 26)
#define SDXC_BOOT_ABORT                (1 << 27)
#define SDXC_VOLTAGE_SWITCH            (1 << 28)
#define SDXC_USE_HOLD_REGISTER        (1 << 29)
#define SDXC_START                    (1 << 31)

/*
 * Interrupt bits
 */
#define SDXC_RESP_ERROR                (1 << 1)
#define SDXC_COMMAND_DONE            (1 << 2)
#define SDXC_DATA_OVER                (1 << 3)
#define SDXC_TX_DATA_REQUEST        (1 << 4)
#define SDXC_RX_DATA_REQUEST        (1 << 5)
#define SDXC_RESP_CRC_ERROR            (1 << 6)
#define SDXC_DATA_CRC_ERROR            (1 << 7)
#define SDXC_RESP_TIMEOUT            (1 << 8)
#define SDXC_DATA_TIMEOUT            (1 << 9)
#define SDXC_VOLTAGE_CHANGE_DONE    (1 << 10)
#define SDXC_FIFO_RUN_ERROR            (1 << 11)
#define SDXC_HARD_WARE_LOCKED        (1 << 12)
#define SDXC_START_BIT_ERROR        (1 << 13)
#define SDXC_AUTO_COMMAND_DONE        (1 << 14)
#define SDXC_END_BIT_ERROR            (1 << 15)
#define SDXC_SDIO_INTERRUPT            (1 << 16)
#define SDXC_CARD_INSERT            (1 << 30)
#define SDXC_CARD_REMOVE            (1 << 31)
#define SDXC_INTERRUPT_ERROR_BIT    (SDXC_RESP_ERROR | SDXC_RESP_CRC_ERROR | SDXC_DATA_CRC_ERROR | SDXC_RESP_TIMEOUT | SDXC_DATA_TIMEOUT | SDXC_FIFO_RUN_ERROR | SDXC_HARD_WARE_LOCKED | SDXC_START_BIT_ERROR | SDXC_END_BIT_ERROR)
#define SDXC_INTERRUPT_DONE_BIT        (SDXC_AUTO_COMMAND_DONE | SDXC_DATA_OVER | SDXC_COMMAND_DONE | SDXC_VOLTAGE_CHANGE_DONE)

/*
 * Status
 */
#define SDXC_RXWL_FLAG                (1 << 0)
#define SDXC_TXWL_FLAG                (1 << 1)
#define SDXC_FIFO_EMPTY                (1 << 2)
#define SDXC_FIFO_FULL                (1 << 3)
#define SDXC_CARD_PRESENT            (1 << 8)
#define SDXC_CARD_DATA_BUSY            (1 << 9)
#define SDXC_DATA_FSM_BUSY            (1 << 10)
#define SDXC_DMA_REQUEST            (1 << 31)
#define SDXC_FIFO_SIZE                (16)

/*
 * Function select
 */
#define SDXC_CEATA_ON                (0xceaa << 16)
#define SDXC_SEND_IRQ_RESPONSE        (1 << 0)
#define SDXC_SDIO_READ_WAIT            (1 << 1)
#define SDXC_ABORT_READ_DATA        (1 << 2)
#define SDXC_SEND_CCSD                (1 << 8)
#define SDXC_SEND_AUTO_STOPCCSD        (1 << 9)
#define SDXC_CEATA_DEV_IRQ_ENABLE    (1 << 10)


struct sdhci_f133_pdata_t
{
    void * virt;
    char * pclk;
    int reset;
    int clk;
    int clkcfg;
    int cmd;
    int cmdcfg;
    int dat0;
    int dat0cfg;
    int dat1;
    int dat1cfg;
    int dat2;
    int dat2cfg;
    int dat3;
    int dat3cfg;
    int dat4;
    int dat4cfg;
    int dat5;
    int dat5cfg;
    int dat6;
    int dat6cfg;
    int dat7;
    int dat7cfg;
    int cd;
    int cdcfg;
};

struct sdhci_cmd_t {
    uint32_t cmdidx;
    uint32_t cmdarg;
    uint32_t resptype;
    uint32_t response[4];
};

struct sdhci_data_t {
    uint8_t * buf;
    uint32_t flag;
    uint32_t blksz;
    uint32_t blkcnt;
};

struct sdhci_t
{
    char * name;
    uint32_t voltages;
    uint32_t width;
    uint32_t clock;
    int removable;
    int isspi;
    void * sdcard;

    int (*detect)(struct sdhci_t * hci);
    int (*reset)(struct sdhci_t * hci);
    int (*setvoltage)(struct sdhci_t * hci, uint32_t voltage);
    int (*setwidth)(struct sdhci_t * hci, uint32_t width);
    int (*setclock)(struct sdhci_t * hci, uint32_t clock);
    int (*transfer)(struct sdhci_t * hci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);
    void * priv;
};

static rt_err_t  f133_transfer_command(struct sdhci_f133_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
    uint32_t cmdval = SDXC_START;
    uint32_t status = 0;
    //ktime_t timeout;
    rt_tick_t cur_tick = 0;
    int kk = 0;

    if(cmd->cmdidx == MMC_STOP_TRANSMISSION)
    {
        cur_tick = rt_tick_get();
        do {
            status = read32(pdat->virt + SD_STAR);

            write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
            write32(pdat->virt + SD_RISR, 0xffffffff);
            
            if(rt_tick_get() - cur_tick > 100)
            {
                //rt_kprintf("zzz1\n");
                return -RT_ETIMEOUT;
            }

            //rt_thread_mdelay(10);
        } while(status & SDXC_CARD_DATA_BUSY);
        return RT_EOK;
    }

    if(cmd->cmdidx == MMC_GO_IDLE_STATE)
        cmdval |= SDXC_SEND_INIT_SEQUENCE;
    if(cmd->resptype & MMC_RSP_PRESENT)
    {
        cmdval |= SDXC_RESP_EXPIRE;
        if(cmd->resptype & MMC_RSP_136)
            cmdval |= SDXC_LONG_RESPONSE;
        if(cmd->resptype & MMC_RSP_CRC)
            cmdval |= SDXC_CHECK_RESPONSE_CRC;
    }

    if(dat)
        cmdval |= SDXC_DATA_EXPIRE | SDXC_WAIT_PRE_OVER;

    if(dat && (dat->flag & MMC_DATA_WRITE))
        cmdval |= SDXC_WRITE;

    if(cmd->cmdidx == MMC_WRITE_MULTIPLE_BLOCK || cmd->cmdidx == MMC_READ_MULTIPLE_BLOCK)
        cmdval |= SDXC_SEND_AUTO_STOP;

    write32(pdat->virt + SD_CAGR, cmd->cmdarg);

    if(dat)
        write32(pdat->virt + SD_GCTL, read32(pdat->virt + SD_GCTL) | 0x80000000);
    write32(pdat->virt + SD_CMDR, cmdval | cmd->cmdidx);

    cur_tick = rt_tick_get();
    do {
        status = read32(pdat->virt + SD_RISR);

        if((status & SDXC_INTERRUPT_ERROR_BIT) || (rt_tick_get() - cur_tick > 100))
        {
            write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
            write32(pdat->virt + SD_RISR, 0xffffffff);
            //rt_kprintf("zzz2\n");
            return  -RT_ERROR;
        }
    } while(!(status & SDXC_COMMAND_DONE));

    if(cmd->resptype & MMC_RSP_BUSY)
    {
        cur_tick = rt_tick_get();
        do {
            status = read32(pdat->virt + SD_STAR);
            if(rt_tick_get() - cur_tick > 100)
            {
                write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
                write32(pdat->virt + SD_RISR, 0xffffffff);
                rt_kprintf("zzz3\n");
                return  -RT_ERROR;
            }
        } while(status & (1 << 9));
    }

    if(cmd->resptype & MMC_RSP_136)
    {
        cmd->response[0] = read32(pdat->virt + SD_RESP3);
        cmd->response[1] = read32(pdat->virt + SD_RESP2);
        cmd->response[2] = read32(pdat->virt + SD_RESP1);
        cmd->response[3] = read32(pdat->virt + SD_RESP0);
    }
    else
    {
        cmd->response[0] = read32(pdat->virt + SD_RESP0);
    }
    write32(pdat->virt + SD_RISR, 0xffffffff);
    return RT_EOK;
}

static rt_err_t read_bytes(struct sdhci_f133_pdata_t * pdat, uint32_t * buf, uint32_t blkcount, uint32_t blksize)
{
    uint64_t count = blkcount * blksize;
    uint32_t * tmp = buf;
    uint32_t status, err, done;

    status = read32(pdat->virt + SD_STAR);
    err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
    while((!err) && (count >= sizeof(uint32_t)))
    {
        if(!(status & SDXC_FIFO_EMPTY))
        {
            *(tmp) = read32(pdat->virt + SD_FIFO);
            tmp++;
            count -= sizeof(uint32_t);
        }
        status = read32(pdat->virt + SD_STAR);
        err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
    }

    do {
        status = read32(pdat->virt + SD_RISR);
        err = status & SDXC_INTERRUPT_ERROR_BIT;
        if(blkcount > 1)
            done = status & SDXC_AUTO_COMMAND_DONE;
        else
            done = status & SDXC_DATA_OVER;
    } while(!done && !err);

    if(err & SDXC_INTERRUPT_ERROR_BIT)
    {
        rt_kprintf("xx2\n");
        return  -RT_ERROR;
    }
        
    write32(pdat->virt + SD_RISR, 0xffffffff);

    if(count)
    {
        rt_kprintf("xx1\n");
        return  -RT_ERROR;
    }
        
    return RT_EOK;
}

static rt_err_t write_bytes(struct sdhci_f133_pdata_t * pdat, uint32_t * buf, uint32_t blkcount, uint32_t blksize)
{
    uint64_t count = blkcount * blksize;
    uint32_t * tmp = buf;
    uint32_t status, err, done;

    status = read32(pdat->virt + SD_STAR);
    err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
    while(!err && count)
    {
        if(!(status & SDXC_FIFO_FULL))
        {
            write32(pdat->virt + SD_FIFO, *tmp);
            tmp++;
            count -= sizeof(uint32_t);
        }
        status = read32(pdat->virt + SD_STAR);
        err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
    }

    do {
        status = read32(pdat->virt + SD_RISR);
        err = status & SDXC_INTERRUPT_ERROR_BIT;
        if(blkcount > 1)
            done = status & SDXC_AUTO_COMMAND_DONE;
        else
            done = status & SDXC_DATA_OVER;
    } while(!done && !err);

    if(err & SDXC_INTERRUPT_ERROR_BIT)
    {
        rt_kprintf("dd2\n");
        return  -RT_ERROR;
    }
        
    write32(pdat->virt + SD_GCTL, read32(pdat->virt + SD_GCTL) | SDXC_FIFO_RESET);
    write32(pdat->virt + SD_RISR, 0xffffffff);

    if(count)
    {
        rt_kprintf("dd1\n");
        return  -RT_ERROR;
    }
        
    return RT_EOK;
}

static rt_err_t f133_transfer_data(struct sdhci_f133_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
    uint32_t dlen = (uint32_t)(dat->blkcnt * dat->blksz);
    rt_err_t ret = RT_EOK;

    write32(pdat->virt + SD_BKSR, dat->blksz);
    write32(pdat->virt + SD_BYCR, dlen);
    if(dat->flag & MMC_DATA_READ)
    {
        if(f133_transfer_command(pdat, cmd, dat))
            return  -RT_ERROR;
        ret = read_bytes(pdat, (uint32_t *)dat->buf, dat->blkcnt, dat->blksz);
    }
    else if(dat->flag & MMC_DATA_WRITE)
    {
        if(f133_transfer_command(pdat, cmd, dat))
            return  -RT_ERROR;
        ret = write_bytes(pdat, (uint32_t *)dat->buf, dat->blkcnt, dat->blksz);
    }
    return RT_EOK;
}

static rt_err_t sdhci_f133_detect(struct sdhci_t * sdhci)
{
    // struct sdhci_f133_pdata_t * pdat = (struct sdhci_f133_pdata_t *)sdhci->priv;

    // if((pdat->cd >= 0) && gpio_get_value(pdat->cd))
    //     return FALSE;
    return RT_EOK;
}

static rt_err_t sdhci_f133_reset(struct sdhci_t * sdhci)
{
    struct sdhci_f133_pdata_t * pdat = (struct sdhci_f133_pdata_t *)sdhci->priv;

    write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
    return RT_EOK;
}

static rt_err_t sdhci_f133_setvoltage(struct sdhci_t * sdhci, uint32_t voltage)
{
    return RT_EOK;
}

static rt_err_t sdhci_f133_setwidth(struct sdhci_t * sdhci, uint32_t width)
{
    struct sdhci_f133_pdata_t * pdat = (struct sdhci_f133_pdata_t *)sdhci->priv;

    switch(width)
    {
    case MMC_BUS_WIDTH_1:
        write32(pdat->virt + SD_BWDR, SDXC_WIDTH1);
        break;
    case MMC_BUS_WIDTH_4:
        write32(pdat->virt + SD_BWDR, SDXC_WIDTH4);
        break;
    case MMC_BUS_WIDTH_8:
        write32(pdat->virt + SD_BWDR, SDXC_WIDTH8);
        break;
    default:
        return  -RT_ERROR;
    }
    return RT_EOK;
}

static rt_err_t sdhci_f133_update_clk(struct sdhci_f133_pdata_t * pdat)
{
    rt_tick_t cur_tick = 0;
    uint32_t cmd = (1U << 31) | (1 << 21) | (1 << 13);

    write32(pdat->virt + SD_CMDR, cmd);
    //ktime_t timeout = ktime_add_ms(ktime_get(), 1);
    cur_tick = rt_tick_get();
    do {
        if((rt_tick_get() - cur_tick) > 100)
        {
            rt_kprintf("update clk err!\n");
            return  -RT_ERROR;
        }
    } while(read32(pdat->virt + SD_CMDR) & 0x80000000);
    write32(pdat->virt + SD_RISR, read32(pdat->virt + SD_RISR));
    return RT_EOK;
}


static rt_err_t  sdhci_f133_setclock(struct sdhci_t * sdhci, uint32_t clock)
{
    struct sdhci_f133_pdata_t * pdat = (struct sdhci_f133_pdata_t *)sdhci->priv;
    uint32_t ratio;
    ratio = 125;
    if((ratio & 0xff) != ratio)
    {
        return  -RT_ERROR;
    }
        
    write32(pdat->virt + SD_CKCR, read32(pdat->virt + SD_CKCR) & ~(1 << 16));
    write32(pdat->virt + SD_CKCR, ratio);
    if(sdhci_f133_update_clk(pdat))
    {
        return  -RT_ERROR;
    }
        
    write32(pdat->virt + SD_CKCR, read32(pdat->virt + SD_CKCR) | (3 << 16));
    if(sdhci_f133_update_clk(pdat))
    {
        return  -RT_ERROR;
    }    
    return RT_EOK;
}

static int sdhci_f133_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
    struct sdhci_f133_pdata_t * pdat = (struct sdhci_f133_pdata_t *)sdhci->priv;
    if(!dat)
        return f133_transfer_command(pdat, cmd, dat);
    return f133_transfer_data(pdat, cmd, dat);
}

void f133_gpio_init(void)
{
    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_0, 3);//D1
    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_1, 3);//D0
    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_2, 3);//CLK
    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_3, 3);//CMD
    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_4, 3);//D3
    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_5, 3);//D2

    d1_set_gpio_mode(GPIO_PORT_F, GPIO_PIN_6, 0);//det 检测
}

static void mmc_request_send(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
    struct sdhci_t *sdhci = (struct sdhci_t *)host->private_data;
    struct sdhci_cmd_t cmd;
    struct sdhci_cmd_t stop;
    struct sdhci_data_t dat;
    rt_memset(&cmd, 0, sizeof(struct sdhci_cmd_t));
    rt_memset(&stop, 0, sizeof(struct sdhci_cmd_t));
    rt_memset(&dat, 0, sizeof(struct sdhci_data_t));

    cmd.cmdidx = req->cmd->cmd_code;
    cmd.cmdarg = req->cmd->arg;
    cmd.resptype =resp_type(req->cmd);
    if (req->data)
    {
        dat.buf = (rt_uint8_t *)req->data->buf;
        dat.flag = req->data->flags;
        dat.blksz = req->data->blksize;
        dat.blkcnt = req->data->blks;

        req->cmd->err = sdhci_f133_transfer(sdhci, &cmd, &dat);
    }
    else
    {
        req->cmd->err = sdhci_f133_transfer(sdhci, &cmd, RT_NULL);
    }

    req->cmd->resp[3] = cmd.response[3];
    req->cmd->resp[2] = cmd.response[2];
    req->cmd->resp[1] = cmd.response[1];
    req->cmd->resp[0] = cmd.response[0];

    if (req->stop)
    {
        stop.cmdidx = req->stop->cmd_code;
        stop.cmdarg = req->stop->arg;
        cmd.resptype =resp_type(req->stop);
        req->stop->err = sdhci_f133_transfer(sdhci, &stop, RT_NULL);
    }

    mmcsd_req_complete(host);
}

static void mmc_set_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
    struct sdhci_t * sdhci = (struct sdhci_t *)host->private_data;
    sdhci_f133_setclock(sdhci, io_cfg->clock);
    sdhci_f133_setwidth(sdhci, io_cfg->bus_width);
}

static const struct rt_mmcsd_host_ops ops =
{
    mmc_request_send,
    mmc_set_iocfg,
    RT_NULL,
    RT_NULL,
};

int f133_sdio_init(void)
{
    struct rt_mmcsd_host * host = RT_NULL;
    struct sdhci_f133_pdata_t * pdat = RT_NULL;
    struct sdhci_t * sdhci = RT_NULL;
    f133_gpio_init();
    host = mmcsd_alloc_host();
    if (!host)
    {
        rt_kprintf("alloc host failed\n");
        goto err;
    }

    sdhci = rt_malloc(sizeof(struct sdhci_t));
    if (!sdhci)
    {
        rt_kprintf("alloc sdhci failed");
        goto err;
    }
    rt_memset(sdhci, 0, sizeof(struct sdhci_t));

    pdat = (struct sdhci_f133_pdata_t *)rt_malloc(sizeof(struct sdhci_f133_pdata_t));
    RT_ASSERT(pdat != RT_NULL);

    pdat->virt = (rt_uint32_t)SMHC0_BASE_ADDR;

    sdhci->name = "sd0";
    sdhci->voltages = VDD_33_34;
    sdhci->width = MMCSD_BUSWIDTH_4;
    sdhci->clock = 1000 * 1000 * 1000;
    sdhci->removable = RT_TRUE;

    sdhci->priv = pdat;
    host->ops = &ops;
    host->freq_min = 400000;
    host->freq_max = 50000000;
    host->valid_ocr = VDD_32_33 | VDD_33_34;
    host->flags = MMCSD_MUTBLKWRITE | MMCSD_SUP_HIGHSPEED | MMCSD_BUSWIDTH_4;
    host->max_seg_size = 2048;
    host->max_dma_segs = 10;
    host->max_blk_size = 512;
    host->max_blk_count = 1;
    host->private_data = sdhci;
    //reset_emmc(pdat);

    mmcsd_change(host);
    return -RT_EOK;

err:
    if (host)  rt_free(host);
    if (sdhci) rt_free(sdhci);

    return RT_EOK;
}

INIT_DEVICE_EXPORT(f133_sdio_init);
