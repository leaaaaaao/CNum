#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"cn_integer.h"

void cn_init_integer(cn_integer n)
{
    n->digits = NULL;
    n->allocd = 0;
    n->size = 0;
}

void cn_int_overflow(cn_integer n)
{
    uint32_t i;
    uint32_t abs_size;
    uint32_t new_size;
    uint32_t *new_vector;

    new_size = (n->allocd == 0) ? (CN_INITIAL_ALLOC) : (n->allocd * 2);

    new_vector = malloc(sizeof(*n->digits) * new_size); 
    MEMCHECK(new_vector);

    abs_size = const_abs(n->size);
    for (i = 0; i < abs_size; i++)
        new_vector[i] = n->digits[i];

    if (n->digits)
        free(n->digits);
    n->digits = new_vector;

    return;
}

int cn_int_equals(cn_integer a, cn_integer b) 
{
    return !cn_int_compare(a, b);
}

int cn_int_compare(cn_integer a, cn_integer b)
{
    if (a->size > b->size)
        return 1;
    if (a->size < b->size)
        return -1;

    /* Numbers are both positive */
    if (a->size > 0)
        return _cn_int_abs_compare(a, b);

    /* Numbers are both non positive. The one with greater abs value is the smallest*/
    return -_cn_int_abs_compare(a, b);
}

int _cn_int_abs_compare(cn_integer a, cn_integer b)
{
    uint32_t a_size;
    uint32_t b_size;
    uint32_t i;

    a_size = const_abs(a->size);
    b_size = const_abs(b->size);
    if (a_size > b_size)
        return 1;
    if (a_size < b_size)
        return -1;

    for (i = a_size - 1; i >= 0; i--) {
        if (a->digits[i] > b->digits[i])
                return 1;
        if (a->digits[i] < b->digits[i])
                return -1;
    }
    return 0;
}

void cn_integer_sum(cn_integer result, cn_integer a, cn_integer b)
{
    /* Same signal, add absolute values */
    if ((a->size >= 0) == (b->size >= 0))
        _cn_add_abs(result, a, b);
    else
        _cn_sub_abs(result, a, b); 

    return;
}

/* Support functions */
void _cn_init_integer_uint32(cn_integer n, uint32_t digit) 
{
    n->digits = malloc(sizeof(*n->digits));
    MEMCHECK(n->digits);

    n->digits[0] = digit;
    n->allocd = 1;
    n->size = 1;
}

/* TODO: REMOVE LATER */
void _cn_push_digit(cn_integer n, uint32_t digit) 
{
    uint32_t abs_size;

    abs_size = abs(n->size);
    if (abs_size == n->allocd)
        cn_int_overflow(n);

    n->digits[abs_size] = digit;
    n->size += (n->size >= 0 ? 1 : -1);
    return;
}

/* From the most significant to the less */
void _cn_print_digits(cn_integer n) 
{
    uint32_t abs_size;
    uint32_t i;

    printf("%s[", n->size < 0 ? "-" : "");
    abs_size = const_abs(n->size);
    for (i = abs_size - 1; i > 0; i--) {
        printf("%u, ", n->digits[i]);
    }
    if (n->size)
        printf("%u", n->digits[0]);
    puts("]");
}

void _cn_realloc_integer(cn_integer n, uint32_t new_size)
{
    uint32_t *new_vector;
    uint32_t abs_size;
    uint32_t i;

    new_vector = malloc(sizeof(*n->digits) * new_size);
    MEMCHECK(new_vector);
    
    abs_size = min(const_abs(n->size), new_size);
    for (i = 0; i < abs_size; i++) {
        new_vector[i] = n->digits[i];
    }

    if(n->digits)
        free(n->digits);

    n->digits = new_vector;
    n->allocd = new_size;
    return;
}

void _cn_add_abs(cn_integer result, cn_integer a, cn_integer b)
{
    uint8_t carry;
    uint32_t a_value;
    uint32_t b_value;
    uint32_t a_size;
    uint32_t b_size;
    uint32_t biggest_size;
    uint32_t sum;
    int signal;
    int i;
    
    biggest_size = max(const_abs(a->size), const_abs(b->size));
    if (result->allocd <= biggest_size)
        _cn_realloc_integer(result, biggest_size + 1);

    carry = 0;
    signal = (a->size < 0) ? -1 : 1;

    result->size = biggest_size * signal;

    a_size = const_abs(a->size);
    b_size = const_abs(b->size);
    for (i = 0; i < biggest_size; i++) {
        a_value = (i < a_size) ? a->digits[i] : 0;
        b_value = (i < b_size) ? b->digits[i] : 0;

        sum = a_value + b_value + carry;
        if (carry == 1 && (sum == a_value || sum == b_value))
            carry = 1;
        else if ((sum < a_value) || (sum < b_value))
            carry = 1;
        else
            carry = 0;
        result->digits[i] = sum;
    }
    
    if (carry) {
        result->size += signal;
        result->digits[i] = 1;
    }
    
    return;
}

void _cn_sub_abs(cn_integer result, cn_integer a, cn_integer b)
{
    uint8_t borrow;
    uint32_t sub;
    uint32_t abs_size;
    uint32_t smallest_size;
    uint32_t a_value;
    uint32_t b_value;
    
    int i;
    int result_signal;

    _CN_INTEGER *biggest;
    _CN_INTEGER *smallest;
    
    biggest = (_cn_int_abs_compare(a, b) > 0) ? a : b;
    smallest = (biggest == a) ? b : a;

    result_signal = (biggest->size < 0) ? -1 : 1;

    abs_size = const_abs(biggest->size);
    smallest_size = const_abs(smallest->size);
    
    if (result->allocd < abs_size)
        _cn_realloc_integer(result, abs_size);
    
    result->size = abs_size * result_signal;
    
    for (i = 0; i < abs_size; i++) {
        a_value = biggest->digits[i];
        b_value = (i < smallest_size) ? smallest->digits[i] : 0; 
        sub = a_value - b_value - borrow;

        result->digits[i] = sub;
        if ((borrow == 1 && sub == a_value) || sub > a_value)
            borrow = 1;
        else
            borrow = 0;
    }

    for (i = abs_size - 1; i >= 0; i--)
    {
        if (result->digits[i] == 0)
            result->size -= result_signal;
        else
            break;
    }

    return;
}
