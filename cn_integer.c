#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<ctype.h>
#include"cn_integer.h"

void cn_init_integer(cn_integer n)
{
    n->digits = NULL;
    n->allocd = 0;
    n->size = 0;
}

int32_t cn_int_equals(cn_integer a, cn_integer b) 
{
    return !cn_int_compare(a, b);
}

/* Return value: -1 if a < b, 1 if a > b and 0 if a == b. Similar to strcmp from string.h */
int32_t cn_int_compare(cn_integer a, cn_integer b)
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

int32_t _cn_int_abs_compare(cn_integer a, cn_integer b)
{
    uint32_t a_size;
    uint32_t b_size;
    int32_t i;

    a_size = CN_INT_SIZE(a);
    b_size = CN_INT_SIZE(b);
    if (a_size > b_size)
        return 1;
    if (a_size < b_size)
        return -1;
    
    if (a_size == 0)
        return 0;

    for (i = a_size - 1; i >= 0; i--) {
        if (a->digits[i] > b->digits[i])
                return 1;
        if (a->digits[i] < b->digits[i])
                return -1;
    }

    return 0;
}

/* Support functions */
void cn_clear_integer(cn_integer n)
{
    if (n->digits)
        free(n->digits);
    n->allocd = 0;
    n->size = 0;
    n->digits = NULL;
}

void cn_int_overflow(cn_integer n)
{
    uint32_t new_allocd;
    uint32_t *new_vec;
    uint32_t abs_size;
    uint32_t i;

    if (n->allocd == 0) {
        new_allocd = CN_INITIAL_ALLOC;
    } else {
        new_allocd = n->allocd * 2;
    }

    new_vec = malloc(sizeof(*n->digits) * new_allocd);
    MEMCHECK(new_vec);

    abs_size = CN_INT_SIZE(n);
    for (i = 0; i < abs_size; i++) {
        new_vec[i] = n->digits[i];
    }

    if (n->digits) {
        free(n->digits);
    }

    n->digits = new_vec;
    n->allocd = new_allocd;
}

void _cn_init_integer_uint32(cn_integer n, uint32_t digit) 
{
    if (digit == 0) {
        cn_init_integer(n);
        return;
    }

    n->digits = malloc(sizeof(*n->digits));
    MEMCHECK(n->digits);

    n->digits[0] = digit;
    n->allocd = 1;
    n->size = 1;
}

/* From the most significant to the less */
void _cn_print_digits(cn_integer n) 
{
    uint32_t abs_size;
    uint32_t i;

    printf("%s[", n->size < 0 ? "-" : "");
    abs_size = CN_INT_SIZE(n);

    if (abs_size == 0) {
        puts("]");
        return;
    }

    for (i = abs_size - 1; i > 0; i--) {
        printf("%u, ", n->digits[i]);
    }
    printf("%u]\n", n->digits[0]);

    return;
}

void _cn_realloc_integer(cn_integer n, uint32_t new_allocd)
{
    uint32_t *new_vector;
    uint32_t abs_size;
    uint32_t i;

    new_vector = malloc(sizeof(*n->digits) * new_allocd);
    MEMCHECK(new_vector);
    
    abs_size = min(CN_INT_SIZE(n), new_allocd);
    for (i = 0; i < abs_size; i++) {
        new_vector[i] = n->digits[i];
    }

    if(n->digits)
        free(n->digits);

    n->digits = new_vector;
    n->allocd = new_allocd;
    return;
}

void cn_integer_assign(cn_integer a, cn_integer b)
{
    uint32_t i;
    uint32_t b_size;

    b_size = CN_INT_SIZE(b);

    while(a->allocd < b_size)
        cn_int_overflow(a);

    a->size = b->size;
    for (i = 0; i < b_size; i++)
        a->digits[i] = b->digits[i];
    return;
}

/* Add operations*/
void cn_int_add(cn_integer result, cn_integer a, cn_integer b)
{
    /* Same signal, add absolute values */
    if ((a->size >= 0) == (b->size >= 0))
        _cn_add_abs(result, a, b);
    else
        _cn_sub_abs(result, a, b);

    return;
}

/* carry must contain the carry from the previous operation on the sum
 * (0 if it is the first), and it will be updated from inside the function */
uint32_t _cn_add_digit(uint32_t a, uint32_t b, uint32_t *carry)
{
    uint32_t sum;

    sum = a + b + *carry;

    if (sum < a || sum < b)
        *carry = 1;
    else if (*carry == 1 && (sum == a || sum == b))
        *carry = 1;
    else
        *carry = 0;

    return sum;
}

void _cn_add_abs(cn_integer result, cn_integer a, cn_integer b)
{
    uint32_t carry;
    uint32_t a_value, b_value;
    uint32_t a_size, b_size;
    uint32_t biggest_size;
    
    int32_t signal;
    uint32_t i;
    
    a_size = CN_INT_SIZE(a);
    b_size = CN_INT_SIZE(b);
    biggest_size = max(b_size, a_size);

    while (result->allocd <= biggest_size)
        cn_int_overflow(result);

    carry = 0;
    signal = CN_INT_SIGNAL(a);

    result->size = biggest_size * signal;
    for (i = 0; i < biggest_size; i++) {
        a_value = (i < a_size) ? a->digits[i] : 0;
        b_value = (i < b_size) ? b->digits[i] : 0;

        result->digits[i] = _cn_add_digit(a_value, b_value, &carry);
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
    uint32_t a_value, b_value;
    
    uint32_t i;
    int32_t result_signal;

    _CN_INTEGER *biggest;
    _CN_INTEGER *smallest;
    
    biggest = (_cn_int_abs_compare(a, b) > 0) ? a : b;
    smallest = (biggest == a) ? b : a;

    result_signal = CN_INT_SIGNAL(biggest);

    abs_size = CN_INT_SIZE(biggest);
    smallest_size = CN_INT_SIZE(smallest);

    if (abs_size == 0) {
        result->size = 0;
        return;
    }

    while (result->allocd < abs_size)
        cn_int_overflow(result);
    
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

    /* Removing leading zeros. Guaranteed abs_size != 0 */
    for (i = abs_size; i > 0; i--) {
        if (result->digits[i - 1] == 0)
            result->size -= result_signal;
        else
            break;
    }

    return;
}

/* Bitwise operations */
void cn_int_shift_digits_right(cn_integer n, uint32_t num_digits)
{
    uint32_t i;
    uint32_t abs_size;
    int32_t sign;

    if (num_digits == 0)
        return;

    sign = CN_INT_SIGNAL(n);
    abs_size = n->size * sign;
    while (n->allocd < abs_size + num_digits)
        cn_int_overflow(n);
    
    n->size += num_digits * sign;

    for (i = abs_size + num_digits - 1; i >= num_digits; i--) {
        n->digits[i] = n->digits[i - num_digits];
    }
    for (i = num_digits; i > 0; i--) {
        n->digits[i - 1] = 0;
    }
 
    return;
}

/* Shift to the "most significance" direction. Talking about digits, it's
 * right; in terms of bits, it's left */
void cn_int_shift_bits_right(cn_integer a, uint32_t num_bits)
{
    uint32_t mask;
    uint32_t abs_size;
    uint32_t i;

    if (num_bits >= 32) {
        cn_int_shift_digits_right(a, num_bits / 32);
        num_bits %= 32;
    }

    abs_size = CN_INT_SIZE(a);
    while (a->allocd <= abs_size)
        cn_int_overflow(a);
    
    /* The (32 - num_bits) least significant bits turned on,
     * leaving the num_bits most significant ones as zeros */
    mask = (1 << (32 - num_bits)) - 1;
    
    /* Only the num_bits most significant bits turned on */
    mask = ~mask;
    
    a->digits[abs_size] = 0;
    for (i = abs_size; i > 0; i--) {
        a->digits[i] <<= num_bits;
        /* The num_bits most significant bits of the next digit become the
         * num_bits least significant of the current one. */
        a->digits[i] |= ((a->digits[i - 1] & mask) >> (32 - num_bits)) & ~mask;
    }
    a->digits[0] <<= num_bits;

    if (a->digits[abs_size] != 0)
        a->size += CN_INT_SIGNAL(a);
    return;
}

/* Product operation */
uint32_t _cn_digit_product(uint32_t digit_a, uint32_t digit_b, uint32_t *carry)
{
    uint32_t result;
    uint32_t mask;
    uint32_t a_parts[2], b_parts[2];
    uint32_t partial_result;
    uint32_t sum_carry;
    uint32_t i;

    mask = 0xFFFF;
    a_parts[0] = digit_a & mask;
    a_parts[1] = (digit_a >> 16) & mask;

    b_parts[0] = digit_b & mask;
    b_parts[1] = (digit_b >> 16) & mask;

    sum_carry = 0;
    result = _cn_add_digit(a_parts[0] * b_parts[0], *carry, &sum_carry);
    *carry = a_parts[1] * b_parts[1] + sum_carry; 

    for (i = 0; i < 2; i++) {
        sum_carry = 0;
        partial_result = a_parts[i] * b_parts[!i];
        
        result = _cn_add_digit(result, (partial_result & mask) << 16, &sum_carry);
        
        *carry += sum_carry;
        *carry += (partial_result & ~mask) >> 16;
     }

    return result;
}


void cn_integer_product(cn_integer result, cn_integer a, cn_integer b)
{
    uint32_t i, j;
    uint32_t a_size, b_size;
    uint32_t t;
    uint32_t carry;
    cn_integer acc;

    a_size = CN_INT_SIZE(a);
    b_size = CN_INT_SIZE(b);

    cn_init_integer(acc);
    _cn_realloc_integer(acc, a_size + b_size);
    for (i = 0; i < a_size + b_size; i++)
        acc->digits[i] = 0;

    acc->size = a_size + b_size;
    acc->size *= CN_INT_SIGNAL(a) * CN_INT_SIGNAL(b);
    for (i = 0; i < a_size; i++) {
        carry = 0;
        for (j = 0; j < b_size; j++) {
            t = _cn_digit_product(a->digits[i], b->digits[j], &carry);
            t += acc->digits[i + j];
            /* Overflow on the sum */
            if (t < acc->digits[i + j]) 
                carry++;

            acc->digits[i + j] = t;
        }
        acc->digits[i + j] = carry;
    }

    for (i = a_size + b_size; i > 0; i--) {
        if (acc->digits[i - 1] == 0)
            acc->size -= CN_INT_SIGNAL(acc);
        else
            break;
    }

    cn_integer_assign(result, acc);
    cn_clear_integer(acc);

    return;
}

/* Fancy things */
void cn_init_integer_from_string(cn_integer n, const char *string)
{
    cn_integer ten, aux;
    int32_t sign;

    cn_init_integer(n);
    _cn_init_integer_uint32(ten, 10);
   
    if (*string == '-')
        sign = -1;
    else
        sign = 1;
    while (isdigit(*string)){
        cn_integer_product(n, n, ten);
        _cn_init_integer_uint32(aux, (*string - '0'));
        cn_int_add(n, n, aux);
        cn_clear_integer(aux);

        string++;
    }

    n->size *= sign;
    cn_clear_integer(ten);

    return;
}
