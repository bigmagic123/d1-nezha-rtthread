
#include <rtthread.h>
#include <string.h>

unsigned long test_addsl(unsigned long b,unsigned long c)
{
  unsigned long a;
  a = b + (c << 9);
  return a;
}

void test_thead_custom(void)
{
    unsigned long test1 = 5;
    unsigned long test2 = 6;
    unsigned long test3 = 0;
    test3 = test_addsl(test1, test2);
    rt_kprintf("aa is %d\n", test3);
}
MSH_CMD_EXPORT(test_thead_custom, test thead custom);