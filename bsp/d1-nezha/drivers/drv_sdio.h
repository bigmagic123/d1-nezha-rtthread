#ifndef __DRV_SDIO_H__
#define __DRV_SDIO_H__

#define SMHC0_BASE_ADDR 0x04020000  //SD
#define SMHC1_BASE_ADDR 0x04021000  //SDIO
#define SMHC2_BASE_ADDR 0x04022000  //EMMC

struct f133_mmc
{
    volatile rt_uint32_t smhc_ctrl_reg;                          /* 0x00 */
    volatile rt_uint32_t smhc_clkdev_reg;                        /* 0x04 */
    volatile rt_uint32_t smhc_tmout_reg;                         /* 0x08 */
    volatile rt_uint32_t smhc_ctype_reg;                         /* 0x0C */
    volatile rt_uint32_t smhc_blksiz_reg;                        /* 0x10 */
    volatile rt_uint32_t smhc_bytcnt_reg;                        /* 0x14 */
    volatile rt_uint32_t smhc_cmd_reg;                           /* 0x18 */
    volatile rt_uint32_t smhc_cmdarg_reg;                        /* 0x1c */
    volatile rt_uint32_t smhc_resp0_reg;                         /* 0x20 */
    volatile rt_uint32_t smhc_resp1_reg;                         /* 0x24 */
    volatile rt_uint32_t smhc_resp2_reg;                         /* 0x28 */
    volatile rt_uint32_t smhc_resp3_reg;                         /* 0x2C */
    volatile rt_uint32_t smhc_intmask_reg;                       /* 0x30 */
    volatile rt_uint32_t smhc_mintsts_reg;                       /* 0x34 */
    volatile rt_uint32_t smhc_rintsts_reg;                       /* 0x38 */
    volatile rt_uint32_t smhc_status_reg;                        /* 0x3C */
    volatile rt_uint32_t smhc_fifoth_reg;                        /* 0x40 */
    volatile rt_uint32_t smhc_funs_reg;                          /* 0x44 */
    volatile rt_uint32_t smhc_tcbcnt_reg;                        /* 0x48 */
    volatile rt_uint32_t smhc_tbbcnt_reg;                        /* 0x4c */
    volatile rt_uint32_t smhc_dbgc_reg;                          /* 0x50 */
    volatile rt_uint32_t smhc_csdc_reg;                          /* 0x54 */
    volatile rt_uint32_t smhc_a12a_reg;                          /* 0x58 */
    volatile rt_uint32_t smhc_ntsr_reg;                          /* 0x5c */
    volatile rt_uint32_t reserved0[5];                           /* reserved: 0x60 - 0x74 */
    volatile rt_uint32_t smhc_hwrst_reg;                         /* 0x78 */
    volatile rt_uint32_t reserved1;                              /* reserved: 0x7c */
    volatile rt_uint32_t smhc_idmac_reg;                         /* 0x80 */
    volatile rt_uint32_t smhc_dlba_reg;                          /* 0x84 */
    volatile rt_uint32_t smhc_idst_reg;                          /* 0x88 */
    volatile rt_uint32_t smhc_idie_reg;                          /* 0x8c */
    volatile rt_uint32_t reserved1[28];                          /* reserved: 0x90 - 0xfc */
    volatile rt_uint32_t smhc_thld_reg;                          /* 0x100 */
    volatile rt_uint32_t smhc_sfc_reg;                           /* 0x104 */
    volatile rt_uint32_t smhc_a23a_reg;                          /* 0x108 */
    volatile rt_uint32_t emmc_ddr_sbit_det;                      /* 0x10c */
    volatile rt_uint32_t reserved2[10];                          /* reserved: 0x110 - 0x134 */
    volatile rt_uint32_t smhc_ext_cmd;                           /* 0x138 */
    volatile rt_uint32_t smhc_ext_resp;                          /* 0x13c */
    volatile rt_uint32_t smhc_drv_dl;                            /* 0x140 */
    volatile rt_uint32_t smhc_smap_dl;                           /* 0x144 */
    volatile rt_uint32_t smhc_ds_dl;                             /* 0x148 */
    volatile rt_uint32_t smhc_hs400_dl;                          /* 0x14c */
    volatile rt_uint32_t reserved3[43];                          /* reserved: 0x150 - 0x1FC */       
    volatile rt_uint32_t smhc_fifo;                              /* 0x200 */
};

typedef struct f133_mmc *f133_mmc_t;

#define SMHC0    ((f133_mmc_t) SMHC0_BASE_ADDR)
#define SMHC1    ((f133_mmc_t) SMHC1_BASE_ADDR)
#define SMHC2    ((f133_mmc_t) SMHC2_BASE_ADDR)

#define BIT(x)              (1<<(x))
/* Struct for Intrrrupt Information */
#define SDXC_RespErr        BIT(1)  /* 0x2 */
#define SDXC_CmdDone        BIT(2)  /* 0x4 */
#define SDXC_DataOver       BIT(3)  /* 0x8 */
#define SDXC_TxDataReq      BIT(4)  /* 0x10 */
#define SDXC_RxDataReq      BIT(5)  /* 0x20 */
#define SDXC_RespCRCErr     BIT(6)  /* 0x40 */
#define SDXC_DataCRCErr     BIT(7)  /* 0x80 */
#define SDXC_RespTimeout    BIT(8)  /* 0x100 */
#define SDXC_ACKRcv         BIT(8)  /* 0x100 */
#define SDXC_DataTimeout    BIT(9)  /* 0x200 */
#define SDXC_BootStart      BIT(9)  /* 0x200 */
#define SDXC_DataStarve     BIT(10) /* 0x400 */
#define SDXC_VolChgDone     BIT(10) /* 0x400 */
#define SDXC_FIFORunErr     BIT(11) /* 0x800 */
#define SDXC_HardWLocked    BIT(12) /* 0x1000 */
#define SDXC_StartBitErr    BIT(13) /* 0x2000 */
#define SDXC_AutoCMDDone    BIT(14) /* 0x4000 */
#define SDXC_EndBitErr      BIT(15) /* 0x8000 */
#define SDXC_SDIOInt        BIT(16) /* 0x10000 */
#define SDXC_CardInsert     BIT(30) /* 0x40000000 */
#define SDXC_CardRemove     BIT(31) /* 0x80000000 */
#define SDXC_IntErrBit      (SDXC_RespErr | SDXC_RespCRCErr | SDXC_DataCRCErr \
                | SDXC_RespTimeout | SDXC_DataTimeout | SDXC_FIFORunErr \
                | SDXC_HardWLocked | SDXC_StartBitErr | SDXC_EndBitErr)  //0xbfc2

/*
    SD CMD reg
REG[0-5] : Cmd ID
REG[6]   : Has response
REG[7]   : Long response
REG[8]   : Check response CRC
REG[9]   : Has data
REG[10]  : Write
REG[11]  : Steam mode
REG[12]  : Auto stop
REG[13]  : Wait previous over
REG[14]  : About cmd
REG[15]  : Send initialization
REG[21]  : Update clock
REG[25:24]: Boot Mode
REG[26]  : Expect boot ack
REG[27]  : Boot abort
REG[28]  : Voltage switch
REG[29]  : Use hold register
REG[31]  : Load cmd
*/
#define SDXC_RESPONSE_CMD        BIT(6)
#define SDXC_LONG_RESPONSE_CMD   BIT(7)
#define SDXC_CHECK_CRC_CMD       BIT(8)
#define SDXC_HAS_DATA_CMD        BIT(9)
#define SDXC_WRITE_CMD           BIT(10)
#define SDXC_STEAM_CMD           BIT(11)
#define SDXC_AUTO_STOP_CMD       BIT(12)
#define SDXC_WAIT_OVER_CMD       BIT(13)
#define SDXC_ABOUT_CMD           BIT(14)
#define SDXC_SEND_INIT_CMD       BIT(15)
#define SDXC_UPDATE_CLOCK_CMD    BIT(21)
#define SDXC_EXPECT_BOOT_ACK_CMD BIT(26)
#define SDXC_BOOT_ABT_CMD        BIT(27)
#define SDXC_VOL_SWITCH_CMD      BIT(28)
#define SDXC_USE_HOLD_CMD        BIT(29)
#define SDXC_LOAD_CMD            BIT(31)

/*
    SD status reg
REG[0]   : FIFO_RX_LEVEL
REG[1]   : FIFO_TX_LEVEL
REG[2]   : FIFO_EMPTY
REG[3]   : FIFO_FULL
REG[4-7] : FSM_STA
REG[8]   : CARD_PRESENT
REG[9]   : CARD_BUSY
REG[10]  : FSM_BUSY
REG[11-16]: RESP_IDX
REG[17-21]: FIFO_LEVEL
REG[31]   : DMA_REQ
*/

#define SDXC_FIFO_RX_LEVEL    BIT(0)
#define SDXC_FIFO_TX_LEVEL    BIT(1)
#define SDXC_FIFO_EMPTY       BIT(2)
#define SDXC_FIFO_FULL        BIT(3)
#define SDXC_CARD_PRESENT     BIT(8)
#define SDXC_CARD_BUSY        BIT(9)
#define SDXC_FSM_BUSY         BIT(10)
#define SDXC_DMA_REQ          BIT(31)

struct mmc_des_v4p1
{
    rt_uint32_t      : 1,
                     dic           : 1, /* disable interrupt on completion                                    */
                     last_des      : 1, /* 1-this data buffer is the last buffer                              */
                     first_des     : 1, /* 1-data buffer is the first buffer,
                        0-data buffer contained in the next descriptor is 1st buffer        */
                     des_chain     : 1, /* 1-the 2nd address in the descriptor is the next descriptor address */
                     //end_of_ring   : 1, /* 1-last descriptor flag when using dual data buffer in descriptor   */
                     : 25,
                     card_err_sum  : 1, /* transfer error flag                                                */
                     own           : 1; /* des owner:1-idma owns it, 0-host owns it                           */

#define SDXC_DES_NUM_SHIFT 12  /* smhc2!! */
#define SDXC_DES_BUFFER_MAX_LEN (1 << SDXC_DES_NUM_SHIFT)
    rt_uint32_t data_buf_sz  : 16,
                data_buf_sz_reserved  : 16;

    rt_uint32_t buf_addr_ptr;
    rt_uint32_t next_des_ptr;
};

#endif