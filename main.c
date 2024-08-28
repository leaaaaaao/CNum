#include<stdio.h>
#include"cn_integer.h"

int main (void)
{
    cn_integer n;
    
    cn_init_integer_from_string(n, "1000000000001420003523500001524000");
    _cn_print_digits(n);
    return 0;
}
