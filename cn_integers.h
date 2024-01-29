#ifndef CN_INTEGERS_H
#define CN_INTEGERS_H

#include <stdint.h>

typedef struct {
	int32_t size;      /* Number of digits actually used, or minus this if the represented number is negative */
	uint32_t *digits;
	uint32_t allocd;   /* Number of digits allocated */
} CN_INT_STRUCT;

typedef CN_INT_STRUCT cn_int[1];

void cn_init_int(cn_int x);
static void cn_digits_realloc(cn_int x, uint32_t n);
static void cn_push_digit(cn_int x, uint32_t n);
void cn_clear_int(cn_int x);
static void cn_add(cn_int result, cn_int a, cn_int b);
void cn_print_digits(cn_int x);

#endif
