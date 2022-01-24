
#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>

#define GT911_ADDR_LEN          2
#define GT911_REGITER_LEN       2
#define GT911_MAX_TOUCH         5
#define GT911_POINT_INFO_NUM   5

#define GT911_ADDRESS_HIGH      0x5D
#define GT911_ADDRESS_LOW       0x14

#define GT911_COMMAND_REG       0x8040
#define GT911_CONFIG_REG        0x8047

#define GT911_PRODUCT_ID        0x8140
#define GT911_VENDOR_ID         0x814A
#define GT911_READ_STATUS       0x814E

#define GT911_POINT1_REG        0x814F
#define GT911_POINT2_REG        0x8157
#define GT911_POINT3_REG        0x815F
#define GT911_POINT4_REG        0x8167
#define GT911_POINT5_REG        0x816F

#define GT911_CHECK_SUM         0x80FF

static struct rt_i2c_client gt911_client;

static rt_err_t gt911_write_reg(struct rt_i2c_client *dev, rt_uint8_t *data, rt_uint8_t len)
{
    struct rt_i2c_msg msgs;

    msgs.addr  = dev->client_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf   = data;
    msgs.len   = len;

    if (rt_i2c_transfer(dev->bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t gt911_read_regs(struct rt_i2c_client *dev, rt_uint8_t *reg, rt_uint8_t *data, rt_uint8_t len)
{
    struct rt_i2c_msg msgs[2];

    msgs[0].addr  = dev->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = reg;
    msgs[0].len   = GT911_REGITER_LEN;

    msgs[1].addr  = dev->client_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = data;
    msgs[1].len   = len;

    if (rt_i2c_transfer(dev->bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t gt911_get_product_id(struct rt_i2c_client *dev, rt_uint8_t *data, rt_uint8_t len)
{
    rt_uint8_t reg[2];

    reg[0] = (rt_uint8_t)(GT911_PRODUCT_ID >> 8);
    reg[1] = (rt_uint8_t)(GT911_PRODUCT_ID & 0xff);

    if (gt911_read_regs(dev, reg, data, len) != RT_EOK)
    {
        rt_kprintf("read id failed");
        return -RT_ERROR;
    }
    return RT_EOK;
}

// static rt_err_t gt911_get_info(struct rt_i2c_client *dev, struct rt_touch_info *info)
// {
//     rt_uint8_t reg[2];
//     rt_uint8_t out_info[7];
//     rt_uint8_t out_len = 7;

//     reg[0] = (rt_uint8_t)(GT911_CONFIG_REG >> 8);
//     reg[1] = (rt_uint8_t)(GT911_CONFIG_REG & 0xFF);

//     if(gt911_read_regs(dev, reg, out_info, out_len) != RT_EOK)
//     {
//         rt_kprintf("read info failed");
//         return -RT_ERROR;
//     }

//     info->range_x = (out_info[2] << 8) | out_info[1];
//     info->range_y = (out_info[4] << 8) | out_info[3];
//     info->point_num = out_info[5] & 0x0f;

//     return RT_EOK;
// }

static rt_err_t gt911_soft_reset(struct rt_i2c_client *dev)
{
    rt_uint8_t buf[3];

    buf[0] = (rt_uint8_t)(GT911_COMMAND_REG >> 8);
    buf[1] = (rt_uint8_t)(GT911_COMMAND_REG & 0xFF);
    buf[2] = 0x02;

    if(gt911_write_reg(dev, buf, 3) != RT_EOK)
    {
        rt_kprintf("soft reset failed");
        return -RT_ERROR;
    }
    return RT_EOK;
}


void test_i2c(void)
{
    uint8_t test_id[8];

    rt_memset(test_id, 0, 8);
    //struct rt_i2c_bus_device * i2c_dev;
    gt911_client.bus = (struct rt_i2c_bus_device *)rt_device_find("i2c0");
    rt_device_open((rt_device_t)gt911_client.bus, RT_DEVICE_FLAG_RDWR);

    gt911_client.client_addr = GT911_ADDRESS_LOW;
    gt911_soft_reset(&gt911_client);

    rt_thread_mdelay(10);
    gt911_get_product_id(&gt911_client, &test_id[0], 4);

    rt_kprintf("%c%c%c%c\n", test_id[0], test_id[1],test_id[2],test_id[3]);

}

MSH_CMD_EXPORT(test_i2c, test i2c);