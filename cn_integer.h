#ifndef CN_INTEGER_H
#define CN_INTEGER_H

#define CN_INITIAL_ALLOC 10
#define MEMCHECK(X) if(NULL == (X)) {fprintf(stderr, "Error: No memory\n"); exit(1);}
#define const_abs(X) (((X) < 0) ? -(X) : (X))
#define max(A, B) (((A) > (B)) ? (A) : (B))
#define min(A, B) (((A) < (B)) ? (A) : (B))

#include<stdint.h>

typedef struct {
    uint32_t *digits;
    uint32_t allocd;
    int32_t size;
} _CN_INTEGER;

typedef _CN_INTEGER cn_integer[1];

void cn_init_integer(cn_integer n);
void cn_int_overflow(cn_integer n);
int cn_int_equals(cn_integer a, cn_integer b);
int cn_int_compare(cn_integer a, cn_integer b);
int _cn_int_abs_compare(cn_integer a, cn_integer b);
void cn_integer_sum(cn_integer result, cn_integer a, cn_integer b);

void _cn_init_integer_uint32(cn_integer n, uint32_t digit);
void _cn_push_digit(cn_integer n, uint32_t digit);
void _cn_print_digits(cn_integer n);
void _cn_realloc_integer(cn_integer n, uint32_t new_size);
void _cn_add_abs(cn_integer result, cn_integer a, cn_integer b);
void _cn_sub_abs(cn_integer result, cn_integer a, cn_integer b);

#endif
