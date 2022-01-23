#include <rtthread.h>
#include <rthw.h>
#include <stdio.h>
#include <string.h>
#include <drv_gpio.h>
#include <drivers/pin.h>

#define LCD_BUF_SIZE  (800 * 480)
rt_uint32_t pixel_ptr[LCD_BUF_SIZE] = {0};

void llll(int gg)
{
    int index = 0;

    if(gg == 0)
    {
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index] = 0xFF0000; //0x07ff;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*1] = 0x808080;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*2] = 0x556B2F;
        }
    }

    if(gg == 1)
    {
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index] =  0xFF1493; //0x07ff;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*1] = 0x191970;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*2] = 0xFFA500;
        }
    }


    if(gg == 2)
    {
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index] = 0xFF0000; //0x07ff;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*1] =0xFFFF00 ;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*2] = 0x9400D3;
        }
    }

    if(gg == 3)
    {
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index] =0x9400D3 ; //0x07ff;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*1] = 0xFF0000;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*2] = 0xFFFF00;
        }
    }

    if(gg == 4)
    {
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index] = 0xFF0000; //0x07ff;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*1] =0x9400D3;
        }
        for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
        {
            pixel_ptr[index + (LCD_BUF_SIZE/3)*2] = 0xFFFF00;
        }
    }
}

static void test_lcd_thread_entry(void* parameter)
{
    int index = 0;
    struct rt_device *lcd;
    struct rt_device_graphic_info *test_lcd_info;

    test_lcd_info = rt_malloc(sizeof( struct rt_device_graphic_info));
 //找到lcd
    lcd = (struct rt_device *)rt_device_find("lcd");
    rt_kprintf("lcd_test\n");

    rt_kprintf("lcd get info:\n");
    rt_device_control(lcd, RTGRAPHIC_CTRL_GET_INFO, test_lcd_info);
    rt_kprintf("lcd width is %d\n", test_lcd_info->width);
    rt_kprintf("lcd height is %d\n", test_lcd_info->height);
    rt_kprintf("lcd bpp is %d\n", test_lcd_info->bits_per_pixel);

#if 0
    for (index = 0; index < (LCD_BUF_SIZE); index ++)
    {
        pixel_ptr[index] = 0xffffff; //0x07ff;
    }
#endif
#if 1
    for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
    {
        pixel_ptr[index] = 0xFFFF00; //0x07ff;
    }
    for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
    {
        pixel_ptr[index + (LCD_BUF_SIZE/3)*1] = 0xFF0000;
    }
    for (index = 0; index < (LCD_BUF_SIZE/3); index ++)
    {
        pixel_ptr[index + (LCD_BUF_SIZE/3)*2] = 0x9400D3;
    }
#endif
    rt_memcpy(test_lcd_info->framebuffer, pixel_ptr, LCD_BUF_SIZE*4);
    rt_thread_delay(10);

#if 1
    int kk = 0;
    while (1)
    {
        kk = kk + 1;
        llll(kk % 5);
        //rt_kprintf("kk is %d:%d\n", kk, kk%5);
        rt_memcpy(test_lcd_info->framebuffer, pixel_ptr, LCD_BUF_SIZE*4);
        //刷新图片
        rt_device_control(lcd, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
        rt_thread_mdelay(100);
    }
#endif
}

void test_lcd()
{
    rt_thread_t lcd_tid = RT_NULL;
    lcd_tid = rt_thread_create("test_lcd",
                            test_lcd_thread_entry, RT_NULL,
                            4096,
                            20, 10);
    if (lcd_tid != RT_NULL)
        rt_thread_startup(lcd_tid);
}
MSH_CMD_EXPORT(test_lcd, test lcd);
