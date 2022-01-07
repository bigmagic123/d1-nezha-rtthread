#include <rtthread.h>
#include <string.h>
#include <math.h>

static void element_add_f32(float *input0, float *input1, float *output, int size)
{
    asm volatile(
                "1:\n\t"
                "vsetvli    t0, %3, e32, m2\n\t"
                "vle.v      v8, (%1)\n\t"
                "sub        %3, %3, t0\n\t"
                "slli       t0, t0, 2\n\t"      // element: 4 bytes
                "add        %1, %1, t0\n\t"
                "vle.v      v12, (%2)\n\t"
                "add        %2, %2, t0\n\t"
                "vfadd.vv   v16, v8, v12\n\t"
                "vse.v      v16, (%0)\n\t"
                "add        %0, %0, t0\n\t"
                "bnez       %3, 1b\n\t"

                :"=r"(output),  // %0
                "=r"(input0),   // %1
                "=r"(input1),   // %2
                "=r"(size)      // %3
                :"0"(output),
                "1"(input0),
                "2"(input1),
                "3"(size)
                : "v8", "v9", "v12", "v13", "v16", "v17", "t0"
    );
}

static void element_sub_f32(float *input0, float *input1, float *output, int size)
{
    asm volatile(
                "1:\n\t"
                "vsetvli    t0, %3, e32, m2\n\t"
                "vle.v      v8, (%1)\n\t"
                "sub        %3, %3, t0\n\t"
                "slli       t0, t0, 2\n\t"      // element: 4 bytes
                "add        %1, %1, t0\n\t"
                "vle.v      v12, (%2)\n\t"
                "add        %2, %2, t0\n\t"
                "vfsub.vv   v16, v8, v12\n\t"
                "vse.v      v16, (%0)\n\t"
                "add        %0, %0, t0\n\t"
                "bnez       %3, 1b\n\t"

                :"=r"(output),  // %0
                "=r"(input0),   // %1
                "=r"(input1),   // %2
                "=r"(size)      // %3
                :"0"(output),
                "1"(input0),
                "2"(input1),
                "3"(size)
                : "v8", "v9", "v12", "v13", "v16", "v17", "t0"
    );
}

static void element_mul_f32(float *input0, float *input1, float *output, int size)
{
    asm volatile(
                "1:\n\t"
                "vsetvli    t0, %3, e32, m2\n\t"
                "vle.v      v8, (%1)\n\t"
                "sub        %3, %3, t0\n\t"
                "slli       t0, t0, 2\n\t"      // element: 4 bytes
                "add        %1, %1, t0\n\t"
                "vle.v      v12, (%2)\n\t"
                "add        %2, %2, t0\n\t"
                "vfmul.vv   v16, v8, v12\n\t"
                "vse.v      v16, (%0)\n\t"
                "add        %0, %0, t0\n\t"
                "bnez       %3, 1b\n\t"

                :"=r"(output),  // %0
                "=r"(input0),   // %1
                "=r"(input1),   // %2
                "=r"(size)      // %3
                :"0"(output),
                "1"(input0),
                "2"(input1),
                "3"(size)
                : "v8", "v9", "v12", "v13", "v16", "v17", "t0"
    );
}

// constrains: The destination address and source address copy do not overlap
// notice: riscv gnu compiler tool-chain c-library memcpy may not use vector inst
// now gcc version: gcc version 10.2.0 (T-HEAD RISCV Tools V2.0.1 B20210512)
void csi_c906_memcpy(void *dst, const void *src, size_t n)
{
    asm volatile(
                "1:\n\t"
                "vsetvli    t0, %3, e8, m4\n\t"
                "vle.v      v4, (%2)\n\t"
                "add        %2, %2, t0\n\t"
                "sub        %3, %3, t0\n\t"
                "vse.v      v4, (%0)\n\t"
                "add        %0, %0, t0\n\t"
                "bnez       %3, 1b\n\t"

                :"=r"(dst)  // %0
                :"0"(dst),  // %1
                "r"(src),   // %2
                "r"(n)      // %3
                : "t0", "v4", "v5", "v6", "v7"
    );
}

void test_v(void)
{
  float a[]={1.1,2.3,3.6,4.2};
  float b[]={1.2,2.4,3.0,4.0};
  float c[]={0.0,0.0,0.0,0.0};
  int len=4;
  int i=0;

  //inline assembly for RVV 0.7.1
  //for(i=0; i<len; i++){c[i]=a[i]+b[i];}
  asm volatile(
               "mv         t4,   %[LEN]       \n\t"
               "mv         t1,   %[PA]        \n\t"
               "mv         t2,   %[PB]        \n\t"
               "mv         t3,   %[PC]        \n\t"
               "LOOP1:                        \n\t"
               "vsetvli    t0,   t4,   e32,m1 \n\t"
               "sub        t4,   t4,   t0     \n\t"
               "slli       t0,   t0,   2      \n\t" //Multiply number done by 4 bytes
               "vle.v      v0,   (t1)         \n\t"
               "add        t1,   t1,   t0     \n\t"
               "vle.v      v1,   (t2)         \n\t"
               "add        t2,   t2,   t0     \n\t"
               "vfadd.vv   v2,   v0,   v1     \n\t"
               "vse.v      v2,   (t3)         \n\t"
               "add        t3,   t3,   t0     \n\t"
               "bnez       t4,   LOOP1        \n\t"
               :
               :[LEN]"r"(len), [PA]"r"(a),[PB]"r"(b),[PC]"r"(c)
               :"cc","memory", "t0", "t1", "t2", "t3", "t4",
                "v0", "v1", "v2"
               );

    for(i=0; i<len; i++){
            rt_kprintf("%d\n",(int)(c[i] * 10));
        }
}


void test_vector_add(void)
{
    int try1 = 10;
    int try2 = 30000;
    int ii = 0;
    float a[10] = {1.2,1.3,1.4,1.5,1.6,1.7,1.8,2.0,2.1,2.2};
    float b[10] = {1.2,1.3,1.4,1.5,1.6,1.7,1.8,2.0,2.1,2.2};
    float c[10];
    float d[10];

    while(try1--)
    {
        element_add_f32(a, b, c, 10);

        for(ii = 0; ii < 10; ii++)
        {
            d[ii] = a[ii] + b[ii];
        }

        for(ii = 0; ii < 10; ii++)
        {
            if(c[ii] != d[ii])
            {
                rt_kprintf("test err!\n");
                break;
            }
        }
    }
    rt_kprintf("add ok!\n");

    rt_tick_t tick_start = rt_tick_get();
    while(try2--)
    {
        element_add_f32(a, b, c, 10);
    }
    rt_kprintf("vector add time is %d ms\n", rt_tick_get() - tick_start);

    try2 = 30000;
    tick_start = rt_tick_get();
    while(try2--)
    {
        for(ii = 0; ii < 10; ii++)
        {
            d[ii] = a[ii] + b[ii];
        }
    }
    rt_kprintf("normal add time is %d ms\n", rt_tick_get() - tick_start);
}


void test_vector_mul(void)
{
    int try1 = 10;
    int try2 = 30000;
    int ii = 0;
    float a[10] = {1.2,1.3,1.4,1.5,1.6,1.7,1.8,2.0,2.1,2.2};
    float b[10] = {1.2,1.3,1.4,1.5,1.6,1.7,1.8,2.0,2.1,2.2};
    float c[10];
    float d[10];

    while(try1--)
    {
        element_mul_f32(a, b, c, 10);

        for(ii = 0; ii < 10; ii++)
        {
            d[ii] = a[ii] * b[ii];
        }

        for(ii = 0; ii < 10; ii++)
        {
            if(c[ii] != d[ii])
            {
                rt_kprintf("test err!\n");
                break;
            }
        }
    }
    rt_kprintf("mul ok!\n");

    rt_tick_t tick_start = rt_tick_get();
    while(try2--)
    {
        element_mul_f32(a, b, c, 10);
    }
    rt_kprintf("vector mul time is %d ms\n", rt_tick_get() - tick_start);

    try2 = 30000;
    tick_start = rt_tick_get();
    while(try2--)
    {
        for(ii = 0; ii < 10; ii++)
        {
            d[ii] = a[ii] * b[ii];
        }
    }
    rt_kprintf("normal mul time is %d ms\n", rt_tick_get() - tick_start);
}

void test_memcpy(void)
{
    int try = 100;
    void *dst = rt_malloc(1024*1024);//1M
    void *src = rt_malloc(1024*1024);
    rt_memset(src, 5, 1024*1024);
    
    rt_kprintf("memcpy ok!\n");
    //rt_memcpy
    rt_tick_t tick_start = rt_tick_get();
    while(try--)
    {
        rt_memcpy(dst, src, 1024*1024);
    }
    rt_kprintf("rt-thread memcpy time is %d ms\n", rt_tick_get() - tick_start);

    try = 100;
    tick_start = rt_tick_get();
    while(try--)
    {
        memcpy(dst, src, 1024*1024);
    }
    rt_kprintf("newlib memcpy time is %d ms\n", rt_tick_get() - tick_start);

    try = 100;
    tick_start = rt_tick_get();
    while(try--)
    {
        csi_c906_memcpy(dst, src, 1024*1024);
    }
    rt_kprintf("vector memcpy time is %d ms\n", rt_tick_get() - tick_start);
}

int vector_init(void)
{
    test_vector_add();
    test_vector_mul();
    test_memcpy();
    test_v();
    return 0;
}
INIT_APP_EXPORT(vector_init);