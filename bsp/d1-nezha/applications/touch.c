/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-13     RiceChen     the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <touch.h>

#define THREAD_PRIORITY   25
#define THREAD_STACK_SIZE 4096
#define THREAD_TIMESLICE  5

static rt_thread_t  gt911_thread = RT_NULL;
static rt_device_t  dev = RT_NULL;
static struct rt_touch_data *read_data;
static struct rt_touch_info info;

static void gt911_entry(void *parameter)
{
    rt_device_control(dev, RT_TOUCH_CTRL_GET_INFO, &info);
    read_data = (struct rt_touch_data *)rt_malloc(sizeof(struct rt_touch_data) * info.point_num);
    while (1)
    {
        if (rt_device_read(dev, 0, read_data, info.point_num) == info.point_num)
        {
            for (rt_uint8_t i = 0; i < info.point_num; i++)
            {
                if (read_data[i].event == RT_TOUCH_EVENT_DOWN || read_data[i].event == RT_TOUCH_EVENT_MOVE)
                {
                    rt_kprintf("%d %d %d %d %d\n", read_data[i].track_id,
                               read_data[i].x_coordinate,
                               read_data[i].y_coordinate,
                               read_data[i].timestamp,
                               read_data[i].width);
                }
            }
        }
    }
}

/* Test function */
int gt911_sample(void)
{
    void *id;
    int x, y;
    dev = rt_device_find("gt911");
    if (dev == RT_NULL)
    {
        rt_kprintf("can't find device: gt911\n");
        return -1;
    }

    if (rt_device_open(dev, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("open device failed!");
        return -1;
    }

    id = rt_malloc(sizeof(rt_uint8_t) * 8);
    rt_device_control(dev, RT_TOUCH_CTRL_GET_ID, id);
    rt_uint8_t * read_id = (rt_uint8_t *)id;
    rt_kprintf("id = GT%d%d%d \n", read_id[0] - '0', read_id[1] - '0', read_id[2] - '0');
    rt_device_control(dev, RT_TOUCH_CTRL_GET_INFO, id);
    rt_kprintf("point_num = %d \n", (*(struct rt_touch_info*)id).point_num);
    rt_free(id);

    gt911_thread = rt_thread_create("gt911",
                                     gt911_entry,
                                     RT_NULL,
                                     THREAD_STACK_SIZE,
                                     THREAD_PRIORITY,
                                     THREAD_TIMESLICE);

    if (gt911_thread != RT_NULL)
        rt_thread_startup(gt911_thread);

    return 0;
}

MSH_CMD_EXPORT(gt911_sample, test gt911);