
#include <string.h>
#include <stdio.h>

__attribute__((interrupt)) void func(void)
{
}

void main(void)
{
    func();
}