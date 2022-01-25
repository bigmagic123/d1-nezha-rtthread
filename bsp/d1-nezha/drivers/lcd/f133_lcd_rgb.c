#include <rtthread.h>
#include <f133_lcd_rgb.h>
#include <drv_gpio.h>
#include <common.h>
#include <cache.h>
uint8_t lcd_fb[LCD_WIDTH * LCD_HEIGHT * 4];

struct f133_tconlcd_reg_t * tcon;

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
    uint32_t val;

    val = read32((void *)&tcon->gctrl);
    val |= (1 << 31);
    write32((void *)&tcon->gctrl, val);
}

static void f133_tconlcd_set_dither(void)
{
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
    int bp, total;
    uint32_t val;
    val = (V_FRONT_PORCH + V_BACK_PORCH + V_SYNC_LEN) / 2;
    write32((void *)&tcon->ctrl, (1 << 31) | (0 << 24) | (0 << 23) | ((val & 0x1f) << 4) | (0 << 0));
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
    uint32_t val;

    val = read32((void *)&tcon->dclk);
    val &= ~(0xf << 28);
    write32((void *)&tcon->dclk, val);

    write32((void *)&tcon->gctrl, 0);
    write32((void *)&tcon->gint0, 0);
}

#define F133_DE   (0x0200160cL)
#define F133_TCON (0x02001b7cL)
#define test_add3 (0x02001600L)

static void reset_f133_assert(void * addr, int offset)
{
    uint32_t val;
	val = read32(addr);
	val &= ~(1 << offset);
	write32(addr, val);
}


static void reset_f133_deassert(void * addr, int offset)
{
	uint32_t val;
    write32(addr, 0x01);
	val = read32(addr);
	val |= (1 << offset);
    val = 0x00010001;
	write32(addr, val);
}

void lcd_gpio_config(void)
{
    //d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_0, 2);//R0
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_1, 2);//R1
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_2, 2);//R2
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_3, 2);//R3
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_4, 2);//R4
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_5, 2);//R5
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_6, 2);//G0
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_7, 2);//G1
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_8, 2);//G2
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_9, 2);//G3
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_10, 2);//G4
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_11, 2);//G5
    //d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_12, 2);//B0
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_13, 2);//B1
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_14, 2);//B2
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_15, 2);//B3
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_16, 2);//B4
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_17, 2);//B5
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_18, 2);//LCD_CLK
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_19, 2);//LCD_DE
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_20, 2);//LCD_HSYNC
    d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_21, 2);//LCD_VSYNC

    if(LCD_BITS_PER_PIXEL == 18)
    {
        d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_0, 2);//R0
        d1_set_gpio_mode(GPIO_PORT_D, GPIO_PIN_12, 2);//B0
    }

    rt_hw_cpu_dcache_invalidate((void*)LCD_TCON_BASE_ADDR, 4096 * 1024);
    rt_hw_cpu_dcache_invalidate((void*)LCD_DE_BASE_ADDR, 4096 * 1024);
    rt_hw_cpu_dcache_invalidate((void*)lcd_fb, LCD_WIDTH * LCD_HEIGHT * 4);

    reset_f133_deassert((void *)F133_DE, 16);
    reset_f133_deassert((void *)F133_TCON, 16);

    write32((void *)0x02001600, 0x81000002);
    write32((void *)0x02001b60, 0x81000002);
    write32((void *)0x02001b9c, 1);

    tcon = (struct f133_tconlcd_reg_t *)((unsigned long long)LCD_TCON_BASE_ADDR);
    f133_tconlcd_disable();
    f133_tconlcd_set_timing();
    f133_tconlcd_set_dither();
    f133_tconlcd_enable();
    f133_de_set_mode();
    f133_de_enable();
    f133_de_set_address((void *)lcd_fb);
    f133_de_enable();
}

rt_uint8_t *lcd_fb_addr_get(void)
{
    return &lcd_fb[0];
}

#define L1_CACHE_BYTES (64)
void cache_inv_range(unsigned long start, unsigned long stop)
{
	register unsigned long i asm("a0") = start & ~(L1_CACHE_BYTES - 1);

	for(; i < stop; i += L1_CACHE_BYTES)
		__asm__ __volatile__(".long 0x02a5000b");	/* dcache.ipa a0 */
	__asm__ __volatile__(".long 0x01b0000b");		/* sync.is */
}

void lcd_show()
{
    cache_inv_range(lcd_fb, LCD_WIDTH * LCD_HEIGHT * 4);
    f133_de_set_address((void *)lcd_fb);
    f133_de_enable();
}

rt_err_t f133_fb_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

rt_err_t f133_fb_close(rt_device_t dev)
{
    return RT_EOK;
}

rt_size_t f133_fb_read(rt_device_t dev, rt_off_t pos, void *buf, rt_size_t size)
{
    return 0;
}

rt_size_t f133_fb_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    return size;
}

rt_err_t f133_fb_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
        {
            struct rt_device_rect_info *info = (struct rt_device_rect_info*)args;
            info = info;
            lcd_show();
        }
        break;

    case RTGRAPHIC_CTRL_GET_INFO:
        {
           struct rt_device_graphic_info* info = (struct rt_device_graphic_info*)args;

            RT_ASSERT(info != RT_NULL);
            info->pixel_format  = RTGRAPHIC_PIXEL_FORMAT_RGB888;
            info->bits_per_pixel= 32;
            info->width         = 800;
            info->height        = 480;
            info->framebuffer   = &lcd_fb[0];
        }
        break;
    }
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops f133_fb_ops =
{
    RT_NULL,
    f133_fb_open,
    f133_fb_close,
    f133_fb_read,
    f133_fb_write,
    f133_fb_control,
};
#endif

int f133_fb_device_init(void)
{
    struct rt_device *device;
    lcd_gpio_config();

    device = rt_malloc(sizeof(struct rt_device));

    /* set device type */
    device->type = RT_Device_Class_Graphic;
    /* initialize device interface */
#ifdef RT_USING_DEVICE_OPS
    device->ops = &f133_fb_ops;
#else
    device->init = RT_NULL;
    device->open = f133_fb_open;
    device->close = f133_fb_close;
    device->read = f133_fb_read;
    device->write = f133_fb_write;
    device->control = f133_fb_control;
#endif

    /* register to device manager */
    rt_device_register(device, "lcd", RT_DEVICE_FLAG_RDWR);

}
INIT_DEVICE_EXPORT(f133_fb_device_init);