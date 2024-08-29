#ifndef CN_INTEGER_H
#define CN_INTEGER_H

#define CN_INITIAL_ALLOC 10
#define MEMCHECK(X) if(NULL == (X)) {fprintf(stderr, "Error: No memory\n"); exit(1);}

#define const_abs(X) (((X) < 0) ? -(X) : (X))
#define max(A, B) (((A) > (B)) ? (A) : (B))
#define min(A, B) (((A) < (B)) ? (A) : (B))

#define CN_INT_SIZE(X) (const_abs(X->size))
#define CN_INT_SIGNAL(X) ((X->size < 0) ? -1 : 1)

#include<stdint.h>

typedef struct {
    uint32_t *digits;
    uint32_t allocd;
    int32_t size; /* abs(size) is the number of digits. size is negative if the number is negative */
} _CN_INTEGER;

typedef _CN_INTEGER cn_integer[1];

void cn_init_integer(cn_integer n);
int32_t cn_int_equals(cn_integer a, cn_integer b);
int32_t cn_int_compare(cn_integer a, cn_integer b);
int32_t _cn_int_abs_compare(cn_integer a, cn_integer b);

/* Support functions */
void cn_clear_integer(cn_integer n);
void cn_int_overflow(cn_integer n);
void _cn_init_integer_uint32(cn_integer n, uint32_t digit);
void _cn_print_digits(cn_integer n); /* Debugging while i don't have division */
void _cn_realloc_integer(cn_integer n, uint32_t new_allocd);
void cn_integer_assign(cn_integer a, cn_integer b);

/* Add operations*/
void cn_int_add(cn_integer result, cn_integer a, cn_integer b);
uint32_t _cn_add_digit(uint32_t a, uint32_t b, uint32_t *carry);
void _cn_add_abs(cn_integer result, cn_integer a, cn_integer b);
void _cn_sub_abs(cn_integer result, cn_integer a, cn_integer b);

/* Bitwise operations */
void cn_int_shift_digits_right(cn_integer n, uint32_t num_digits);
void cn_int_shift_bits_right(cn_integer a, uint32_t num_bits);

/* Product operation */
uint32_t _cn_digit_product(uint32_t digit_a, uint32_t digit_b, uint32_t * carry);
void cn_integer_product(cn_integer result, cn_integer a, cn_integer b);

/* Fancy things */
void cn_init_integer_from_string(cn_integer n, const char *string);

#endif
