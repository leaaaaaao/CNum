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
void cn_clear_integer(cn_integer n)
{
    if (n->size != 0)
        free(n->digits);
    n->allocd = 0;
    n->size = 0;
    n->digits = NULL;
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
    
    a_size = const_abs(a->size);
    b_size = const_abs(b->size);
    biggest_size = max(b_size, a_size);

    if (result->allocd <= biggest_size)
        _cn_realloc_integer(result, biggest_size + 1);

    carry = 0;
    signal = (a->size < 0) ? -1 : 1;

    result->size = biggest_size * signal;
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

    for (i = abs_size - 1; i >= 0; i--) {
        if (result->digits[i] == 0)
            result->size -= result_signal;
        else
            break;
    }

    return;
}

void cn_int_shift_digits_right(cn_integer n, uint32_t num_digits)
{
    int i;
    uint32_t abs_size;
    int sign;

    sign = (n->size < 0) ? -1 : 1;
    abs_size = n->size * sign;
    if (n->allocd < abs_size + num_digits)
        _cn_realloc_integer(n, abs_size +  num_digits);
    n->size += num_digits * sign;

    for (i = abs_size + num_digits - 1; i >= (int)num_digits; i--) {
        n->digits[i] = n->digits[i - num_digits];
    }
    for (i = num_digits - 1; i >= 0; i--) {
        n->digits[i] = 0;
    }   
 
    return;
}

void cn_int_shift_bits_right(cn_integer a, uint32_t num_bits)
{
    uint32_t mask;
    uint32_t abs_size;
    int i;
    if (num_bits > 32) {
        cn_int_shift_digits_right(a, num_bits / 32);
        num_bits %= 32;
    }

    abs_size = const_abs(a->size);
    if (a->allocd <= abs_size)
        _cn_realloc_integer(a, abs_size + 1);
    mask = 0;
    for (i = 0; i < 32 - num_bits; i++) {
        mask <<= 1;
        mask |= 1;
    }
    /* num_bits most significant bits turned on */
    mask = ~mask;
    
    a->digits[abs_size] = 0;
    for (i = abs_size; i > 0; i--) {
        a->digits[i] <<= num_bits;
        a->digits[i] |= ((a->digits[i - 1] & mask) >> (32 - num_bits)) & ~mask;
    }
    a->digits[0] <<= num_bits;

    if (a->digits[abs_size] != 0)
        a->size += (a->size < 0) ? -1 : 1;
    return;
}

void cn_integer_assign(cn_integer a, cn_integer b)
{
    int i;
    if (a->allocd != 0) {
        free(a->digits);
    }

    a->digits = malloc(sizeof(*a->digits) * b->size);
    MEMCHECK(a->digits);

    a->allocd = b->size;
    a->size = b->size;
    for (i = 0; i < b->size; i++) {
        a->digits[i] = b->digits[i];
    }
    return;
}

void _cn_digit_product(cn_integer result, uint32_t digit_a, uint32_t digit_b)
{
    uint16_t a_parts[2];
    uint16_t b_parts[2]; 
    uint32_t result_partials[4];
    uint16_t mask;
    cn_integer aux[2];
    int i;

    if (result->allocd < 2) {
       _cn_realloc_integer(result, 2); 
    }

    mask = 0xFFFF;
    a_parts[0] = digit_a & mask;
    a_parts[1] = (digit_a >> 16) & mask;

    b_parts[0] = digit_b & mask;
    b_parts[1] = (digit_b >> 16) & mask;

    for (i = 0; i < 4; i++) {
        result_partials[i] = (uint32_t) a_parts[(i & 2) >> 1] * (uint32_t) b_parts[i & 1];
    }
    
    /* Add the first and less significant partial without shifts
     * and the last and most significant partial shifted by one digit. */
    _cn_init_integer_uint32(aux[0], result_partials[0]);
    _cn_init_integer_uint32(aux[1], result_partials[3]);
    
    cn_int_shift_digits_right(aux[1], 1);
    cn_integer_sum(aux[0], aux[0], aux[1]);

    cn_clear_integer(aux[1]);

    /* Add the intermediate partials both shifted by 16 bits */
    for (i = 1; i <= 2; i++) {
        _cn_init_integer_uint32(aux[1], result_partials[i]);
        cn_int_shift_bits_right(aux[1], 16);
        cn_integer_sum(aux[0], aux[0], aux[1]);
        cn_clear_integer(aux[1]);    
    }

    cn_integer_assign(result, aux[0]);
    cn_clear_integer(aux[0]);
    if (result->digits[1] == 0) {
        if (result->digits[0] == 0)
            result->size = 0;
        else
            result->size = 1;
    } else {
        result->size = 2;
    }
    return;
}

void cn_integer_product(cn_integer result, cn_integer a, cn_integer b)
{
    int i, j, sign;
    uint32_t a_size, b_size;
    cn_integer accumulator;
    cn_integer aux;

    cn_init_integer(aux);
    _cn_init_integer_uint32(accumulator, 0);

    a_size = const_abs(a->size);
    b_size = const_abs(b->size);

    for (i = 0; i < a_size; i++) {
        for (j = 0; j < b_size; j++) {
            _cn_digit_product(aux, a->digits[i], b->digits[j]);
            cn_int_shift_digits_right(aux, i + j);
            cn_integer_sum(accumulator, accumulator, aux);
            cn_clear_integer(aux);
        }
    }

    cn_integer_assign(result, accumulator);
    result->size *= ((a->size < 0) == (b->size < 0)) ? 1 : -1;
    cn_clear_integer(accumulator);
    return;
}

void cn_init_integer_from_string(cn_integer n, const char *string)
{
    cn_integer accumulator, ten, aux;
    cn_init_integer(accumulator);
    _cn_init_integer_uint32(ten, 10);
    
    while (isdigit(*string)){
        cn_integer_product(accumulator, accumulator, ten);
        _cn_init_integer_uint32(aux, (*string - '0'));
        cn_integer_sum(accumulator, accumulator, aux);
        cn_clear_integer(aux);

        string++;
    }

    cn_integer_assign(n, accumulator);
    cn_clear_integer(accumulator);
    cn_clear_integer(ten);

    return;
}
