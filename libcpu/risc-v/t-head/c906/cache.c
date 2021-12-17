/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-29     lizhirui     first version
 * 2021-11-05     JasonHu      add c906 cache inst
 */

#include <rthw.h>
#include <rtdef.h>
#include <board.h>
#include <riscv.h>

#define L1_CACHE_BYTES (64)

static void dcache_wb_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        /* asm volatile("dcache.cva %0\n"::"r"(i):"memory"); */
        /*
         * compiler always use a5 = i.
         * a6 not used, so we use a6 here.
         */
        asm volatile("mv a6, %0\n"::"r"(i):"memory");   /* a6 = a5(i) */
        asm volatile(".long 0x0257800b");               /* dcache.cva a6 */
    }
    asm volatile(".long 0x01b0000b");   /* sync.is */
}

static void dcache_inv_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        /* asm volatile("dcache.iva %0\n"::"r"(i):"memory"); */
        asm volatile("mv a6, %0\n"::"r"(i):"memory");   /* a6 = a5(i) */
        asm volatile(".long 0x0268000b");               /* dcache.iva a6 */
    }
    asm volatile(".long 0x01b0000b");
}

static void dcache_wbinv_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        /* asm volatile("dcache.civa %0\n"::"r"(i):"memory"); */
        asm volatile("mv a6, %0\n"::"r"(i):"memory");   /* a6 = a5(i) */
        asm volatile(".long 0x0278000b");               /* dcache.civa a6 */
    }
    asm volatile(".long 0x01b0000b");
}

static void icache_inv_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        /* asm volatile("icache.iva %0\n"::"r"(i):"memory"); */
        asm volatile("mv a6, %0\n"::"r"(i):"memory");   /* a6 = a5(i) */
        asm volatile(".long 0x0308000b");               /* icache.iva a6 */
    }
    asm volatile(".long 0x01b0000b");
}

rt_inline rt_uint32_t rt_cpu_icache_line_size(void)
{
    return L1_CACHE_BYTES;
}

rt_inline rt_uint32_t rt_cpu_dcache_line_size(void)
{
    return L1_CACHE_BYTES;
}

void rt_hw_cpu_icache_invalidate(void *addr,int size)
{
    icache_inv_range((unsigned long)addr, (unsigned long)((unsigned char *)addr + size));
}

void rt_hw_cpu_dcache_invalidate(void *addr,int size)
{
    dcache_inv_range((unsigned long)addr, (unsigned long)((unsigned char *)addr + size));
}

void rt_hw_cpu_dcache_clean(void *addr,int size)
{
    dcache_wb_range((unsigned long)addr, (unsigned long)((unsigned char *)addr + size));
}

void rt_hw_cpu_dcache_clean_flush(void *addr,int size)
{
    dcache_wbinv_range((unsigned long)addr, (unsigned long)((unsigned char *)addr + size));
}

void rt_hw_cpu_icache_ops(int ops,void *addr,int size)
{
    if(ops == RT_HW_CACHE_INVALIDATE)
    {
        rt_hw_cpu_icache_invalidate(addr, size);
    }
}

void rt_hw_cpu_dcache_ops(int ops,void *addr,int size)
{
    if(ops == RT_HW_CACHE_FLUSH)
    {
        rt_hw_cpu_dcache_clean(addr, size);
    }
    else
    {
        rt_hw_cpu_dcache_invalidate(addr, size);
    }
}

void rt_hw_cpu_dcache_clean_all(void)
{
    /* asm volatile("dcache.call\n":::"memory"); */
    asm volatile(".long 0x0010000b\n":::"memory");
}

void rt_hw_cpu_dcache_invalidate_all(void)
{
    /* asm volatile("dcache.ciall\n":::"memory"); */
    asm volatile(".long 0x0030000b\n":::"memory");
}

void rt_hw_cpu_icache_invalidate_all(void)
{
    /* asm volatile("icache.iall\n":::"memory"); */
    asm volatile(".long 0x0100000b\n":::"memory");
}

int sys_cacheflush(void *addr, int size, int cache)
{
    return 0;
}
