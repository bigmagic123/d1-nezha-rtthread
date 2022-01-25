#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <string.h>
#include <drv_i2c.h>
#include <common.h>
#include <drv_gpio.h>
#include <rtdevice.h>
#include <drivers/i2c.h>

enum {
	I2C_STAT_BUS_ERROR	= 0x00,
	I2C_STAT_TX_START	= 0x08,
	I2C_STAT_TX_RSTART	= 0x10,
	I2C_STAT_TX_AW_ACK	= 0x18,
	I2C_STAT_TX_AW_NAK	= 0x20,
	I2C_STAT_TXD_ACK	= 0x28,
	I2C_STAT_TXD_NAK	= 0x30,
	I2C_STAT_LOST_ARB	= 0x38,
	I2C_STAT_TX_AR_ACK	= 0x40,
	I2C_STAT_TX_AR_NAK	= 0x48,
	I2C_STAT_RXD_ACK	= 0x50,
	I2C_STAT_RXD_NAK	= 0x58,
	I2C_STAT_IDLE		= 0xf8,
};


static int f133_i2c_wait_status(void)
{
    rt_tick_t cur_tick = rt_tick_get();

    int ret = 0;
    while(1)
    {
        if((read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) & (1 << 3)))
        {
            ret = read32(TWI2_ADDR_BASE + TWI_STAT_OFFSET);
            break;
        }

        if(rt_tick_get() - cur_tick > 1)
        {
            ret = I2C_STAT_BUS_ERROR;
            break;
        }
    }

    return ret;
}


static int f133_i2c_start(void)
{
	rt_uint32_t val;
    rt_tick_t cur_tick = rt_tick_get();

	val = read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET);
	val |= (1 << 5) | (1 << 3);
	write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, val);

    while(1)
    {
        if(!(read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) & (1 << 5)))
        {
            break;
        }
			
        if(rt_tick_get() - cur_tick > 100)
        {
            break;
        }
    }
    return f133_i2c_wait_status();
}

static int f133_i2c_stop(void)
{
    rt_uint32_t val;
    rt_tick_t cur_tick = rt_tick_get();

	val = read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET);
	val |= (1 << 4) | (1 << 3);
	write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, val);

    while(1)
    {
        if(!(read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) & (1 << 4)))
			break;
        if(rt_tick_get() - cur_tick > 100)
        {
            break;
        }
    }
    return f133_i2c_wait_status();
}

static int f133_i2c_send_data(uint8_t dat)
{
	write32(TWI2_ADDR_BASE + TWI_DATA_OFFSET, dat);
	write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) | (1 << 3));
	return f133_i2c_wait_status();
}

static int f133_i2c_write(struct rt_i2c_msg * msg)
{
	uint8_t * p = msg->buf;
	int len = msg->len;

	if(f133_i2c_send_data((uint8_t)(msg->addr << 1)) != I2C_STAT_TX_AW_ACK)
		return -1;
	while(len > 0)
	{
		if(f133_i2c_send_data(*p++) != I2C_STAT_TXD_ACK)
        {
            return -1;
        }
			
		len--;
	}
	return 0;
}

static int f133_i2c_read(struct rt_i2c_msg * msg)
{
	rt_uint8_t * p = msg->buf;
	int len = msg->len;
    int ret = len;
    uint8_t ss;

	if(f133_i2c_send_data((rt_uint8_t)(msg->addr << 1 | 1)) != I2C_STAT_TX_AR_ACK)
    {
        return -1;
    }
		
	write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) | (1 << 2));
	while(len > 0)
	{
		if(len == 1)
		{
			write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, (read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) & ~(1 << 2)) | (1 << 3));
			if(f133_i2c_wait_status() != I2C_STAT_RXD_NAK)
            {
                return -1;
            }
		}
		else
		{
			write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, read32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET) | (1 << 3));
			if(f133_i2c_wait_status() != I2C_STAT_RXD_ACK)
            {
                return -1;
            }
		}
        ss = read32(TWI2_ADDR_BASE + TWI_DATA_OFFSET);
		*p++ = ss;
		len--;
	}
	return ret;
}

void i2c_gpio_init(void)
{
    //PE12
    //twi2-sck 2
    //PE13 
    //twi2-sda 2
    d1_set_gpio_mode(GPIO_PORT_E, GPIO_PIN_12, 2);//SCK
    d1_set_gpio_mode(GPIO_PORT_E, GPIO_PIN_13, 2);//SDA

    d1_set_gpio_pull(GPIO_PORT_E, GPIO_PIN_12, 1);
    d1_set_gpio_pull(GPIO_PORT_E, GPIO_PIN_13, 1);
 
}

void i2c_set_rate(void)
{
    write32((void *)(TWI2_ADDR_BASE + TWI_CCR_OFFSET), 0x28);//400k
}

static rt_size_t f133_i2c_mst_xfer(struct rt_i2c_bus_device *bus,
                                    struct rt_i2c_msg msgs[],
                                    rt_uint32_t num)
{

    rt_uint32_t res = 0;
    rt_uint32_t i = 0;

    if(f133_i2c_start() != I2C_STAT_TX_START)
    {
        return 0;
    }

    for (i = 0; i < num; i++)
    {

        if(i != 0)
		{
			if(f133_i2c_start() != I2C_STAT_TX_RSTART)
				break;
		}

        if (msgs[i].flags & RT_I2C_RD)
            res = f133_i2c_read(&msgs[i]);
        else
            res = f133_i2c_write(&msgs[i]);
    }
    
    f133_i2c_stop();

    return i;
}
static rt_size_t f133_i2c_slv_xfer(struct rt_i2c_bus_device *bus,
                                    struct rt_i2c_msg msgs[],
                                    rt_uint32_t num)
{
    return 0;
}

static rt_err_t f133_i2c_bus_control(struct rt_i2c_bus_device *bus,
                                      rt_uint32_t cmd,
                                      rt_uint32_t arg)
{
    return RT_EOK;
}

static const struct rt_i2c_bus_device_ops f133_i2c_ops =
{
    .master_xfer = f133_i2c_mst_xfer,
    .slave_xfer = f133_i2c_slv_xfer,
    .i2c_bus_control = f133_i2c_bus_control,
};

struct f133_i2c_hw_config
{
    rt_uint32_t twi_num;
    rt_uint32_t twi_rate;
    rt_uint32_t twi_address;
    rt_uint32_t sda_pin;
    rt_uint32_t scl_pin;
    rt_uint32_t sda_mode;
    rt_uint32_t scl_mode;
};

static struct f133_i2c_hw_config hw_device =
{
    .twi_num = 2,
    .twi_rate = 400000,//400k
    .twi_address = TWI2_ADDR_BASE,
    .sda_pin = GPIO_PORT_E,
    .scl_pin = GPIO_PORT_E,
    .sda_mode = 2,
    .scl_mode = 2,
};

struct rt_i2c_bus_device f133_i2c_dev =
{
    .ops = &f133_i2c_ops,
    .priv =  (void *)&hw_device,
};

void i2c_init(void)
{
    write32((void *)0x0200191c, (0x1 << 18)|(0x1<< 2));
    i2c_gpio_init();
    i2c_set_rate();
    write32(TWI2_ADDR_BASE + TWI_CNTR_OFFSET, 1 << 6);
	write32(TWI2_ADDR_BASE + TWI_SRST_OFFSET, 1 << 0);

    rt_i2c_bus_device_register(&f133_i2c_dev, "i2c0");
}

INIT_BOARD_EXPORT(i2c_init);