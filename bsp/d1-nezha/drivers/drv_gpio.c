#include <stdio.h>
#include <stdlib.h>
#include "drv_gpio.h"
#include "common.h"
#include <rtthread.h>
#include <drivers/pin.h>

//d1s have 6 group gpio
//6 groups of ports (PB(6), PC(6), PD(23), PE(14), PF(7), PG(16))
// PB2 - PB7
// PC2 - PC7
// PD0 - PD22
// PE0 - PE13
// PF0 - PF6
// PG0 - PG15

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
    uint32_t gpio_base_addr = 0;
    uint32_t cur_val = 0;
    gpio_base_addr = gpio_port + 0x10;
    cur_val = read32(gpio_base_addr);

    if(val)
    {
        cur_val |= (1 << gpio_pin);
    }
    else
    {
        cur_val &= ~(1 << gpio_pin);
    }
    
    write32(gpio_base_addr, cur_val);
}

uint8_t d1_get_gpio_val(uint32_t gpio_port, uint32_t gpio_pin)
{
    uint32_t gpio_base_addr = 0;
    uint32_t cur_val = 0;
    gpio_base_addr = gpio_port + 0x10;
    cur_val = read32(gpio_base_addr);
    uint8_t ret = 0; 

    if(cur_val & (1 << gpio_pin))
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

void d1_set_gpio_irq_enable(uint32_t gpio_port, uint32_t gpio_pin, uint32_t cfg, uint8_t enable)
{
    uint32_t pin_level = 0;
    uint32_t gpio_base_addr = 0;
    uint32_t val = 0;
    pin_level = gpio_pin / 8;
    gpio_base_addr = D1_GPIO_BASE + D1_GPIO_PB_EINT_CFG0 + pin_level * 0x04;
    val = read32(gpio_base_addr);

    val &= ~(0xf << ((gpio_pin & 0x7) << 2));
    val |= ((cfg & 0x0f) << ((gpio_pin & 0x7) << 2));

    write32(gpio_base_addr, val);

    //set gpio irq enable
    gpio_base_addr = D1_GPIO_BASE + D1_GPIO_PB_EINT_CTL;
    val = read32(gpio_base_addr);
    if(enable)
    {
        val |= (1 << gpio_pin);
    }
    else
    {
        val &= ~(1 << gpio_pin);
    }
    write32(gpio_base_addr, val);
}

uint32_t d1_get_pb_irq_status(void)
{
    uint32_t val = 0;
    uint32_t gpio_base_addr = 0;
    gpio_base_addr = D1_GPIO_BASE + D1_GPIO_PB_EINT_STATUS;
    val = read32(gpio_base_addr);

    write32(gpio_base_addr, val);//write 1 clear
    return val;
}


static void get_gpio_from_pin(rt_base_t pin, uint32_t* cur_gpio_port, uint32_t* cur_gpio_pin)
{
    if(pin < 6)//PB2 - PB7
    {
        *cur_gpio_port = GPIO_PORT_B;
        *cur_gpio_pin = pin + 2;
    }
    else if(pin < 12)//PC2 - PC7
    {
        *cur_gpio_port = GPIO_PORT_C;
        *cur_gpio_pin = pin - 6 + 2;
    }
    else if(pin < 35)//PD0 - PD22
    {
        *cur_gpio_port = GPIO_PORT_D;
        *cur_gpio_pin = pin - 12;
    }
    else if(pin < 49)//PE0 - PE13
    {
        *cur_gpio_port = GPIO_PORT_E;
        *cur_gpio_pin = pin - 35;
    }
    else if(pin < 56)//PF0 - PF6
    {
        *cur_gpio_port = GPIO_PORT_F;
        *cur_gpio_pin = pin - 49;
    }
    else if(pin < 72)//PG0 - PG15
    {
        *cur_gpio_port = GPIO_PORT_G;
        *cur_gpio_pin = pin - 56;
    }
}

static void d1s_pin_mode(struct rt_device *dev, rt_base_t pin, rt_base_t mode)
{
    GPIO_FUNC d1s_mode = OUTPUT;
    uint32_t cur_gpio_port = 0;
    uint32_t cur_gpio_pin = 0;

   get_gpio_from_pin(pin, &cur_gpio_port, &cur_gpio_pin);

    switch (mode)
    {
    case PIN_MODE_OUTPUT:
        d1s_mode = OUTPUT;
        break;
    case PIN_MODE_INPUT:
        d1s_mode = INPUT;
        break;
    case PIN_MODE_INPUT_PULLUP://to do
        d1s_mode = INPUT;
        break;
    case PIN_MODE_INPUT_PULLDOWN://to do
        d1s_mode = INPUT;
        break;
    case PIN_MODE_OUTPUT_OD:
        d1s_mode = OUTPUT;
        break;
    }
    d1_set_gpio_mode(cur_gpio_port, cur_gpio_pin, d1s_mode);
}

static void d1s_pin_write(struct rt_device *dev, rt_base_t pin, rt_base_t value)
{
    uint32_t cur_gpio_port = 0;
    uint32_t cur_gpio_pin = 0;

    get_gpio_from_pin(pin, &cur_gpio_port, &cur_gpio_pin);

    d1_set_gpio_val(cur_gpio_port, cur_gpio_pin, value);

}

static int d1s_pin_read(struct rt_device *device, rt_base_t pin)
{
    uint32_t pin_level = 0;

    uint32_t cur_gpio_port = 0;
    uint32_t cur_gpio_pin = 0;

    get_gpio_from_pin(pin, &cur_gpio_port, &cur_gpio_pin);

    pin_level = d1_get_gpio_val(cur_gpio_port, cur_gpio_pin);
    return pin_level;
}

static rt_err_t d1s_pin_attach_irq(struct rt_device *device, rt_int32_t pin, rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    return RT_EOK;
}

static rt_err_t d1s_pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
    return RT_EOK;
}

rt_err_t d1s_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint32_t enabled)
{
    return RT_EOK;
}


rt_base_t d1s_pin_get(const char *name)
{

    rt_base_t pin = 0;
    int hw_port_num, hw_pin_num = 0;
    int i, name_len;

    name_len = rt_strlen(name);

    if ((name_len < 4) || (name_len >= 6))
    {
        return -RT_EINVAL;
    }
    if ((name[0] != 'P') || (name[2] != '.'))
    {
        return -RT_EINVAL;
    }

    if(name[1] == 'B')
    {
        pin = 0;
    }
    else if(name[1] == 'C')
    {
        pin = 6;
    }
    else if(name[1] == 'D')
    {
        pin = 12;
    }
    else if(name[1] == 'E')
    {
        pin = 35;
    }
    else if(name[1] == 'F')
    {
        pin = 49;
    }
    else if(name[1] == 'G')
    {
        pin = 56; 
    }

    for (i = 3; i < name_len; i++)
    {
        hw_pin_num *= 10;
        hw_pin_num += name[i] - '0';
    }

    if(hw_pin_num > 23)
    {
        return -RT_EINVAL;
    }

    return pin + hw_pin_num;
}

static const struct rt_pin_ops ops =
{
    d1s_pin_mode,
    d1s_pin_write,
    d1s_pin_read,
    d1s_pin_attach_irq,
    d1s_pin_detach_irq,
    d1s_pin_irq_enable,
    d1s_pin_get,
};

int rt_hw_gpio_init(void)
{
    rt_device_pin_register("gpio", &ops, RT_NULL);
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_gpio_init);
