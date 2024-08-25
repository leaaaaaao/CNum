#include<stdio.h>
#include"cn_integer.h"

int main (void)
{
    cn_integer a;
    cn_integer b;
    cn_integer c;

    _cn_init_integer_uint32(a, ~0 - 5);
    _cn_init_integer_uint32(b, 6);
    cn_init_integer(c);
   
    a->size = -a->size;
    b->size = -b->size; 
    cn_integer_sum(c, a, b);

    printf("A: ");
    _cn_print_digits(a);
    printf("B: ");
    _cn_print_digits(b);
    printf("A + B = C: ");
    _cn_print_digits(c);

    return 0;
}
