
#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>

#include <riscv-vector.h>

vint16m1_t test_vaddvv_int16xm1 (vint16m1_t a, vint16m1_t b) 
{
    return a + b;
}

void test_auto_rvv()
{
    vint16m1_t aa = {1};
    vint16m1_t bb = {1};

    test_vaddvv_int16xm1(aa, bb);
}
MSH_CMD_EXPORT(test_auto_rvv, test auto rvv);