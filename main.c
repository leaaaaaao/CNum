#include<stdio.h>
#include"cn_integer.h"

int main (void)
{
    cn_integer n;

    /* 100!, according to python */
    cn_init_integer_from_string(n, "93326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976156518286253697920827223758251185210916864000000000000000000000000");
    
    printf("100! = ");
    _cn_print_digits(n);
    puts("(in base 2^32)");
    return 0;
}
