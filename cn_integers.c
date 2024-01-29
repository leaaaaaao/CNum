#include <stdio.h>
#include <stdlib.h>
#include "cn_integers.h"

#define INITSIZE 10
#define MEMCHECK(X) if(NULL == (X)) {fprintf(stderr, "ERROR: Out of memory\n"); exit(1);}

#define MAX(a, b) ((a) > (b)) ? (a) : (b);
#define MIN(a, b) ((a) < (b)) ? (b) : (a);

void cn_init_int(cn_int x)
{
	x->size = 0;
	x->allocd = INITSIZE;
	x->digits = malloc(sizeof(*(x->digits)) * INITSIZE);
	MEMCHECK(x->digits);	
	return;
}

static void cn_digits_realloc(cn_int x, uint32_t n)
{
	uint32_t size;
	uint32_t i;
	uint32_t *newArray;

	size = MIN(x->size, n);
	x->allocd = n;
	newArray = malloc(sizeof(*(x->digits)) * n);
	MEMCHECK(newArray);

	for(i = 0; i < size; i++)
		newArray[i] = x->digits[i];

	free(x->digits);
	x->digits = newArray;
	return;
}

static void cn_push_digit(cn_int x, uint32_t n)
{
	if (x->size == x->allocd)
		cn_digits_realloc(x, x->allocd * 2);

	x->digits[x->size++] = n;
	return;
}

void cn_clear_int(cn_int x)
{
	free(x->digits);
	x->allocd = 0;
	x->size = 0;
	return;
}

static void cn_add(cn_int result, cn_int a, cn_int b)
{
	uint32_t i, maxSize, minSize, carry;
	uint32_t *maxArray;
	
	minSize = MIN(a->size, b->size);
	maxSize = MAX(a->size, b->size);

	if (result->allocd < maxSize + 1)
		cn_digits_realloc(result, maxSize + 1);

	carry = 0;
	for (i = 0; i < minSize; i++) {
		result->digits[i] = a->digits[i] + b->digits[i] + carry;
		carry = result->digits[i] - carry < a->digits[i] || (result->digits[i] == 0 && carry);
	}

	maxArray = (maxSize == a->size) ? a->digits : b->digits;
	for ( ; i < maxSize; i++) {
		result->digits[i] = maxArray[i] + carry;
		carry = result->digits[i] - carry < maxArray[i] || (result->digits[i] == 0 && carry);
	}
	
	result->size = maxSize + carry;	
	result->digits[i] = carry;

	return;
}

void cn_print_digits(cn_int x)
{
	uint32_t i, size;

	size = x->size;

	puts("");

	for(i = size; i > 0; i--)
		printf("%u ", x->digits[i - 1]);

	puts("");

	return;
}

int main (void) {
	/* Tests */
	uint32_t i;
	cn_int a, b, r;

	cn_init_int(a);
	cn_init_int(b);
	cn_init_int(r);
	
	for (i = 0; i < 5; i++) {
		cn_push_digit(a, (i % 2) ? 0 : ~0);
	}
	
	cn_push_digit(b, ~0);
	cn_push_digit(b, ~0);

	cn_add(r, a, b);
	cn_print_digits(a);
	cn_print_digits(b);

	cn_print_digits(r);
	return 0;
}
