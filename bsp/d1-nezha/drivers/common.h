#ifndef COMMON_H__
#define COMMON_H__

#include "rtthread.h"

rt_inline rt_uint32_t read32(void *addr)
{
    return (*((volatile unsigned int *)(addr)));
}

rt_inline void write32(void *addr, rt_uint32_t value)
{
    (*((volatile unsigned int *)(addr))) = value;
}

rt_inline uint64_t counter(void)
{
    uint64_t cnt;
    __asm__ __volatile__("csrr %0, time\n" : "=r"(cnt) :: "memory");
    return cnt;
}

rt_inline void sdelay(unsigned long us)
{
    uint64_t t1 = counter();
    uint64_t t2 = t1 + us * 24;
    do {
        t1 = counter();
    } while(t2 >= t1);
}

#endif