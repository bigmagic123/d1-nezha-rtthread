#ifndef F133_LCD_RGB_H__
#define F133_LCD_RGB_H__

#define LCD_DE_BASE_ADDR     (0x05000000)
#define LCD_TCON_BASE_ADDR   (0x05461000)

#define LCD_WIDTH             (800)
#define LCD_HEIGHT            (480)

#define LCD_PHYSICAL_WIDTH             (216)
#define LCD_PHYSICAL_HEIGHT            (135)

#define LCD_BITS_PER_PIXEL             (18)
#define LCD_BYTES_PER_PIXEL            (4)

#define PIXEL_CLOCK_HZ                 (33000000)
#define H_FRONT_PORCH                  (40)
#define H_BACK_PORCH                   (87)
#define V_FRONT_PORCH                  (13)
#define V_BACK_PORCH                   (31)
#define V_SYNC_LEN                     (1)
#define H_SYNC_LEN                     (1)
#define H_SYNC_ACTIVE                  (0)
#define V_SYNC_ACTIVE                  (0)
#define DEN_ACTIVE                     (1)
#define CLK_ACTIVE                     (1)

struct f133_tconlcd_reg_t {
    uint32_t gctrl;                   /* 0x00 */
    uint32_t gint0;                   /* 0x04 */
    uint32_t gint1;                   /* 0x08 */
    uint32_t res_0c;
    uint32_t frm_ctrl;                /* 0x10 */
    uint32_t frm_seed[6];             /* 0x14 */
    uint32_t frm_table[4];            /* 0x2c */
    uint32_t fifo_3d;                 /* 0x3c */
    uint32_t ctrl;                    /* 0x40 */
    uint32_t dclk;                    /* 0x44 */
    uint32_t timing0;                 /* 0x48 */
    uint32_t timing1;                 /* 0x4c */
    uint32_t timing2;                 /* 0x50 */
    uint32_t timing3;                 /* 0x54 */
    uint32_t hv_intf;                 /* 0x58 */
    uint32_t res_5c;
    uint32_t cpu_intf;                /* 0x60 */
    uint32_t cpu_wr;                  /* 0x64 */
    uint32_t cpu_rd0;                 /* 0x68 */
    uint32_t cpu_rd1;                 /* 0x6c */
    uint32_t res_70_80[5];            /* 0x70 */
    uint32_t lvds_intf;               /* 0x84 */
    uint32_t io_polarity;             /* 0x88 */
    uint32_t io_tristate;             /* 0x8c */
    uint32_t res_90_f8[27];
    uint32_t debug;                   /* 0xfc */
    uint32_t ceu_ctl;                 /* 0x100 */
    uint32_t res_104_10c[3];
    uint32_t ceu_coef[20];            /* 0x110 */
    uint32_t cpu_tri0;                /* 0x160 */
    uint32_t cpu_tri1;                /* 0x164 */
    uint32_t cpu_tri2;                /* 0x168 */
    uint32_t cpu_tri3;                /* 0x16c */
    uint32_t cpu_tri4;                /* 0x170 */
    uint32_t cpu_tri5;                /* 0x174 */
    uint32_t res_178_17c[2];
    uint32_t cmap_ctl;                /* 0x180 */
    uint32_t res_184_18c[3];
    uint32_t cmap_odd0;               /* 0x190 */
    uint32_t cmap_odd1;               /* 0x194 */
    uint32_t cmap_even0;              /* 0x198 */
    uint32_t cmap_even1;              /* 0x19c */
    uint32_t res_1a0_1ec[20];
    uint32_t safe_period;             /* 0x1f0 */
    uint32_t res_1f4_21c[11];
    uint32_t lvds_ana0;               /* 0x220 */
    uint32_t lvds_ana1;               /* 0x224 */
    uint32_t res_228_22c[2];
    uint32_t sync_ctl;                /* 0x230 */
    uint32_t sync_pos;                /* 0x234 */
    uint32_t slave_stop_pos;          /* 0x238 */
    uint32_t res_23c_3fc[113];
    uint32_t gamma_table[256];        /* 0x400 */
};


#define F133_DE_MUX_GLB			(0x00100000 + 0x00000)
#define F133_DE_MUX_BLD			(0x00100000 + 0x01000)
#define F133_DE_MUX_CHAN		(0x00100000 + 0x02000)
#define F133_DE_MUX_VSU			(0x00100000 + 0x20000)
#define F133_DE_MUX_GSU1		(0x00100000 + 0x30000)
#define F133_DE_MUX_GSU2		(0x00100000 + 0x40000)
#define F133_DE_MUX_GSU3		(0x00100000 + 0x50000)
#define F133_DE_MUX_FCE			(0x00100000 + 0xa0000)
#define F133_DE_MUX_BWS			(0x00100000 + 0xa2000)
#define F133_DE_MUX_LTI			(0x00100000 + 0xa4000)
#define F133_DE_MUX_PEAK		(0x00100000 + 0xa6000)
#define F133_DE_MUX_ASE			(0x00100000 + 0xa8000)
#define F133_DE_MUX_FCC			(0x00100000 + 0xaa000)
#define F133_DE_MUX_DCSC		(0x00100000 + 0xb0000)

struct de_clk_t {
	uint32_t gate_cfg;
	uint32_t bus_cfg;
	uint32_t rst_cfg;
	uint32_t div_cfg;
	uint32_t sel_cfg;
};

struct de_glb_t {
	uint32_t ctl;
	uint32_t status;
	uint32_t dbuff;
	uint32_t size;
};

struct de_bld_t {
	uint32_t fcolor_ctl;
	struct {
		uint32_t fcolor;
		uint32_t insize;
		uint32_t offset;
		uint32_t dum;
	} attr[4];
	uint32_t dum0[15];
	uint32_t route;
	uint32_t premultiply;
	uint32_t bkcolor;
	uint32_t output_size;
	uint32_t bld_mode[4];
	uint32_t dum1[4];
	uint32_t ck_ctl;
	uint32_t ck_cfg;
	uint32_t dum2[2];
	uint32_t ck_max[4];
	uint32_t dum3[4];
	uint32_t ck_min[4];
	uint32_t dum4[3];
	uint32_t out_ctl;
};

struct de_vi_t {
	struct {
		uint32_t attr;
		uint32_t size;
		uint32_t coord;
		uint32_t pitch[3];
		uint32_t top_laddr[3];
		uint32_t bot_laddr[3];
	} cfg[4];
	uint32_t fcolor[4];
	uint32_t top_haddr[3];
	uint32_t bot_haddr[3];
	uint32_t ovl_size[2];
	uint32_t hori[2];
	uint32_t vert[2];
};

struct de_ui_t {
	struct {
		uint32_t attr;
		uint32_t size;
		uint32_t coord;
		uint32_t pitch;
		uint32_t top_laddr;
		uint32_t bot_laddr;
		uint32_t fcolor;
		uint32_t dum;
	} cfg[4];
	uint32_t top_haddr;
	uint32_t bot_haddr;
	uint32_t ovl_size;
};

void lcd_show();

#endif