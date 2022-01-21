#include <rtthread.h>
#include <f133_lcd_rgb.h>
#include <drv_gpio.h>
#include <common.h>

uint8_t lcd_fb[LCD_WIDTH * LCD_HEIGHT * 4];
//uint8_t lcd_fb[64];

static void fb_setbl(void)
{
    //设置背光

	// struct fb_f133_rgb_pdata_t * pdat = (struct fb_f133_rgb_pdata_t *)fb->priv;
	// led_set_brightness(pdat->backlight, brightness);
}

static void inline f133_de_enable(void)
{
	struct de_glb_t * glb = (struct de_glb_t *)(LCD_DE_BASE_ADDR + F133_DE_MUX_GLB);
	write32((void *)&glb->dbuff, 1);
}

static inline void f133_de_set_address(void *fb)
{
	struct de_ui_t * ui = (struct de_ui_t *)(LCD_DE_BASE_ADDR + F133_DE_MUX_CHAN + 0x1000 * 1);
	write32((void *)&ui->cfg[0].top_laddr, (uint32_t)(unsigned long)fb);
}

static inline void f133_de_set_mode(void)
{
	struct de_clk_t * clk = (struct de_clk_t *)(LCD_DE_BASE_ADDR);
	struct de_glb_t * glb = (struct de_glb_t *)(LCD_DE_BASE_ADDR + F133_DE_MUX_GLB);
	struct de_bld_t * bld = (struct de_bld_t *)(LCD_DE_BASE_ADDR + F133_DE_MUX_BLD);
	struct de_ui_t * ui = (struct de_ui_t *)(LCD_DE_BASE_ADDR + F133_DE_MUX_CHAN + 0x1000 * 1);
	uint32_t size = (((LCD_HEIGHT - 1) << 16) | (LCD_WIDTH - 1));
	uint32_t val;
	int i;

	val = read32((void *)&clk->rst_cfg);
	val |= 1 << 0;
	write32((void *)&clk->rst_cfg, val);

	val = read32((void *)&clk->gate_cfg);
	val |= 1 << 0;
	write32((void *)&clk->gate_cfg, val);

	val = read32((void *)&clk->bus_cfg);
	val |= 1 << 0;
	write32((void *)&clk->bus_cfg, val);

	val = read32((void *)&clk->sel_cfg);
	val &= ~(1 << 0);
	write32((void *)&clk->sel_cfg, val);

	write32((void *)&glb->ctl, (1 << 0));
	write32((void *)&glb->status, 0);
	write32((void *)&glb->dbuff, 1);
	write32((void *)&glb->size, size);

	for(i = 0; i < 4; i++)
	{
		void * chan = (void *)(LCD_DE_BASE_ADDR + F133_DE_MUX_CHAN + 0x1000 * i);
		memset(chan, 0, i == 0 ? sizeof(struct de_vi_t) : sizeof(struct de_ui_t));
	}
	memset(bld, 0, sizeof(struct de_bld_t));

	write32((void *)&bld->fcolor_ctl, 0x00000101);
	write32((void *)&bld->route, 1);
	write32((void *)&bld->premultiply, 0);
	write32((void *)&bld->bkcolor, 0xff000000);
	write32((void *)&bld->bld_mode[0], 0x03010301);
	write32((void *)&bld->bld_mode[1], 0x03010301);
	write32((void *)&bld->output_size, size);
	write32((void *)&bld->out_ctl, 0);
	write32((void *)&bld->ck_ctl, 0);
	for(i = 0; i < 4; i++)
	{
		write32((void *)&bld->attr[i].fcolor, 0xff000000);
		write32((void *)&bld->attr[i].insize, size);
	}

	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_VSU, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_GSU1, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_GSU2, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_GSU3, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_FCE, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_BWS, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_LTI, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_PEAK, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_ASE, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_FCC, 0);
	write32(LCD_DE_BASE_ADDR + F133_DE_MUX_DCSC, 0);

	write32((void *)&ui->cfg[0].attr, (1 << 0) | (4 << 8) | (1 << 1) | (0xff << 24));
	write32((void *)&ui->cfg[0].size, size);
	write32((void *)&ui->cfg[0].coord, 0);
	write32((void *)&ui->cfg[0].pitch, 4 * LCD_WIDTH);
	write32((void *)&ui->cfg[0].top_laddr, (rt_uint32_t)(unsigned long)lcd_fb);
	write32((void *)&ui->ovl_size, size);
}

static void f133_tconlcd_enable(void)
{
	struct f133_tconlcd_reg_t * tcon = (struct f133_tconlcd_reg_t *)LCD_TCON_BASE_ADDR;
	uint32_t val;

	val = read32((void *)&tcon->gctrl);
	val |= (1 << 31);
	write32((void *)&tcon->gctrl, val);
}

static void f133_tconlcd_set_dither(void)
{
	struct f133_tconlcd_reg_t * tcon = (struct f133_tconlcd_reg_t *)LCD_TCON_BASE_ADDR;

	if((LCD_BITS_PER_PIXEL == 16) || (LCD_BITS_PER_PIXEL == 18))
	{
		write32((void *)&tcon->frm_seed[0], 0x11111111);
		write32((void *)&tcon->frm_seed[1], 0x11111111);
		write32((void *)&tcon->frm_seed[2], 0x11111111);
		write32((void *)&tcon->frm_seed[3], 0x11111111);
		write32((void *)&tcon->frm_seed[4], 0x11111111);
		write32((void *)&tcon->frm_seed[5], 0x11111111);
		write32((void *)&tcon->frm_table[0], 0x01010000);
		write32((void *)&tcon->frm_table[1], 0x15151111);
		write32((void *)&tcon->frm_table[2], 0x57575555);
		write32((void *)&tcon->frm_table[3], 0x7f7f7777);

		if(LCD_BITS_PER_PIXEL == 16)
			write32((void *)&tcon->frm_ctrl, (1 << 31) | (1 << 6) | (0 << 5)| (1 << 4));
		else if(LCD_BITS_PER_PIXEL == 18)
			write32((void *)&tcon->frm_ctrl, (1 << 31) | (0 << 6) | (0 << 5)| (0 << 4));
	}
}

static void f133_tconlcd_set_timing(void)
{
	struct f133_tconlcd_reg_t * tcon = (struct f133_tconlcd_reg_t *)LCD_TCON_BASE_ADDR;
	int bp, total;
	uint32_t val;

	val = (V_FRONT_PORCH + V_BACK_PORCH + V_SYNC_LEN) / 2;
	write32((void *)&tcon->ctrl, (1 << 31) | (0 << 24) | (0 << 23) | ((val & 0x1f) << 4) | (0 << 0));
	//val = clk_get_rate(pdat->clk_tconlcd) / pdat->timing.pixel_clock_hz;
    //printf("val is %d\n", val);
    val = 12;
	write32((void *)&tcon->dclk, (0xf << 28) | ((val / 2) << 0));
	write32((void *)&tcon->timing0, ((LCD_WIDTH - 1) << 16) | ((LCD_HEIGHT - 1) << 0));
	bp = H_SYNC_LEN + H_BACK_PORCH;
	total = LCD_WIDTH + H_FRONT_PORCH + bp;
	write32((void *)&tcon->timing1, ((total - 1) << 16) | ((bp - 1) << 0));
	bp = V_SYNC_LEN + V_BACK_PORCH;
	total = LCD_HEIGHT + V_FRONT_PORCH + bp;
	write32((void *)&tcon->timing2, ((total * 2) << 16) | ((bp - 1) << 0));
	write32((void *)&tcon->timing3, ((H_SYNC_LEN - 1) << 16) | ((V_SYNC_LEN - 1) << 0));

	val = (0 << 31) | (1 << 28);
	if(!H_SYNC_ACTIVE)
		val |= (1 << 25);
	if(!V_SYNC_ACTIVE)
		val |= (1 << 24);
	if(!DEN_ACTIVE)
		val |= (1 << 27);
	if(!CLK_ACTIVE)
		val |= (1 << 26);
	write32((void *)&tcon->io_polarity, val);
	write32((void *)&tcon->io_tristate, 0);
}

static void f133_tconlcd_disable(void)
{
	struct f133_tconlcd_reg_t * tcon = (struct f133_tconlcd_reg_t *)LCD_TCON_BASE_ADDR;
	uint32_t val;

	val = read32((void *)&tcon->dclk);
	val &= ~(0xf << 28);
	write32((void *)&tcon->dclk, val);

	write32((void *)&tcon->gctrl, 0);
	write32((void *)&tcon->gint0, 0);
}

void lcd_gpio_config(void)
{
    //d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_0, 3);//R0
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_1, 3);//R1
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_2, 3);//R2
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_3, 3);//R3
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_4, 3);//R4
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_5, 3);//R5
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_6, 3);//G0
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_7, 3);//G1
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_8, 3);//G2
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_9, 3);//G3
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_10, 3);//G4
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_11, 3);//G5
    //d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_12, 3);//B0
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_13, 3);//B1
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_14, 3);//B2
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_15, 3);//B3
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_16, 3);//B4
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_17, 3);//B5
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_18, 3);//LCD_CLK
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_19, 3);//LCD_DE
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_20, 3);//LCD_HSYNC
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_21, 3);//LCD_VSYNC

    if(LCD_BITS_PER_PIXEL == 18)
    {
        d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_0, 3);//R0
        d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_12, 3);//B0
    }

    f133_tconlcd_disable();
    f133_tconlcd_set_timing();
    f133_tconlcd_set_dither();
    f133_de_set_mode();
    f133_de_enable();
	f133_de_set_address((void *)lcd_fb);
	f133_de_enable();

    //show
    fb_setbl();
    //rt_uint32_t lcd_mem = rt_malloc(LCD_WIDTH * LCD_HEIGHT * LCD_BYTES_PER_PIXEL);
}

rt_uint8_t *lcd_fb_addr_get(void)
{
    return &lcd_fb[0];
}

void lcd_show()
{
    f133_de_set_address((void *)lcd_fb);
	f133_de_enable();
}

void fb_f133_rgb_init(void)
{
    lcd_gpio_config();
}