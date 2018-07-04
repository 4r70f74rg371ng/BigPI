/*
BigInt.h: BigInt's Header file
*/

// header is all decalared functions, decalared variables
// does not decalared again
// so we need to prevension twice inculsion
// this is method 1 of prevention twice inclusion:  
// by  define a const if we redefine, than we reinclusion
// and we can not redecalre all functions variables follow in this file 
#ifndef _BIG_INT_H_
#define _BIG_INT_H_

// this is method 2 of prevention twice inclusion: 
// by using compiler preprocessor 'pragma' to setting this file will not inclusion twice
//#pragma once

// C Boolean variable
#ifndef BOOL
#define BOOL int
#endif

// C Boolean True
#ifndef TRUE
#define TRUE 1
#endif

// C Boolean False
#ifndef FALSE
#define FALSE 0
#endif

// C null
#ifndef NULL
#define NULL ((void*)0)
#endif // NULL

// BigInt digital
#define __bigint_digital  9ULL

// BigInt base
#define __bigint_base  1000000000ULL

// arr_type
#ifdef __GNUC__
#include <stdint.h>
typedef int64_t arrtype;
#else
typedef __int64 arrtype;
#endif

/*
BigInt Data Structure

magic integer (also useful, meaningful):
base == __bigint_base;
digital == __bigint_digital;
*/
typedef struct _BigInt {
	/*
	base: base of 'arr' array digital
	ex:
	arr = {2,3,4}
	base = 10
	=> bint = 2*10^2+3*10+4 = 234
	*/
	arrtype base;

	/*
	digital: maximum digital count of one integer {=floor(log_10(base-1))+1}, used in printf-like functions
	ex:
	base = 123
	arr[n] = 0,1,2,3,4,5,6,7,8,9,.....,122 => arr = {(0..122), ..., (0..122)}
	digital = 3 {floor(log_10(base-1))+1=floor(log_10(122))+1=
	floor(log_10(100)+ep)+1 = floor(2.***)+1=2+1=3}

	*/
	arrtype digital;

	/*
	arr: array of integer digitals, little endine
	ex:
	arr = {67, 45, 23}
	base = 100
	=> bint = 23*100^2+45*100+67 = 234567
	ex2:
	arr = {4, 3, 2}
	base = 100
	=> bint = 2*100^2+3*100+4 = 20304 (020304)
	*/
	arrtype* arr;
	/*
	arrlen: array length
	ex:
	arr = {67, 45, 23}
	arrlen = 3 (=log_base(bint))
	ex2:
	arr = {67, 45, 23, 0}
	arrlen = 4 (but log_base(bint) = 3, need to find head)
	*/
	arrtype arrlen;

	/*
	sign: sign of integer (positive/negtive)
	ex:
	+/- => 0/1, -inf/inf => -2/2
	ex2:
	sign=1, arr={2,3,4}, base=100
	=> bint = -20304
	*/
	arrtype sign;
}BigInt;

#define __bigint_magic_assert(pbigint) (((pbigint) != NULL) && ((pbigint)->base == __bigint_base) && ((pbigint)->digital == __bigint_digital))

// initial functions
BigInt* bigint_initial_dec_string(char* sinteger);
BigInt* bigint_initial_hex_string(char* hexstr); // [+]
BigInt* bigint_initial_one(arrtype bintb);
BigInt* bigint_initial_ten_base_one(arrtype bintb, arrtype basen); // bintb*10^basen    [+]
BigInt* bigint_initial_from_array(arrtype sign, arrtype* arr, arrtype size);
BigInt* bigint_initial_zero();
BigInt* bigint_initial_size(arrtype size);
BigInt* bigint_duplicate(BigInt* bigint); // == initial from anothor

//logic functions
BOOL bigint_isneed_normalize(BigInt* bint);
int bigint_compare(BigInt* a, BigInt* b);
int bigint_compare_one(BigInt* a, arrtype bintb);
int bigint_iszero(BigInt* bigint);

// print functions
char* bigint_print_return_string(BigInt* bigint);
void bigint_print(BigInt* bigint);
void bigint_print_info(BigInt* bigint);

void bigint_cleanup(BigInt* bigint);
void bigint_cleanup_pointer(BigInt** bigint);

// special functions
void bigint_normalize(BigInt* bint);
arrtype bigint_length(BigInt* bigint);
void bigint_replace(BigInt* a, BigInt* b); // for result assign to same variable
arrtype bigint_base10_length(BigInt* bigint); // [+]
void bigint_write_digital(BigInt* bigint, arrtype index, arrtype value); // [+]
arrtype bigint_read_digital(BigInt* bigint, arrtype index); // [+]

BigInt* bigint_add(BigInt* a, BigInt* b);
void bigint_add_into(BigInt* a, BigInt* b);
BigInt* bigint_add_one(BigInt* a, arrtype b);
void bigint_add_one_into(BigInt* a, arrtype b);
BigInt* bigint_add_one_internal(BigInt* a, arrtype b);
BigInt* bigint_add_return_internal(BigInt* a, BigInt* b);
char* add_dec_integer(char* sbinta, char* sbintb);
char* add_dec_integer_one(char* sbinta, arrtype bintb);

BigInt* bigint_del(BigInt* a, BigInt* b);
void bigint_del_into(BigInt* a, BigInt* b);
BigInt* bigint_del_one(BigInt* a, arrtype b);
void bigint_del_one_into(BigInt* a, arrtype b);
BigInt* bigint_del_one_internal(BigInt* a, arrtype b);
BigInt* bigint_del_return_internal(BigInt* a, BigInt* b);
char* del_dec_integer(char* sbinta, char* sbintb);
char* del_dec_integer_one(char* sbinta, arrtype bintb);

BigInt* bigint_mul(BigInt* a, BigInt* b);
void bigint_mul_into(BigInt* a, BigInt* b);
BigInt* bigint_mul_one(BigInt* a, arrtype b);
void bigint_mul_one_into(BigInt* a, arrtype b);
char* mul_dec_integer(char* sbinta, char* sbintb);
char* mul_dec_integer_one(char* sbinta, arrtype bintb);

BigInt** bigint_div(BigInt* a, BigInt* b);
void bigint_div_into(BigInt* a, BigInt* b);
BigInt** bigint_div_one(BigInt* a, arrtype bintb);
void bigint_div_one_into(BigInt* a, arrtype bintb);
char** div_dec_integer(char* sbinta, char* sbintb);
char** div_dec_integer_one(char* sbinta, arrtype bintb);

// special function for div
BigInt* bigint_div_return_quotient(BigInt* a, BigInt* b);
BigInt* bigint_div_one_return_quotient(BigInt* a, arrtype bintb);
BigInt* bigint_div_return_reminder(BigInt* a, BigInt* b);
BigInt* bigint_div_one_return_reminder(BigInt* a, arrtype bintb);
void bigint_div_into_reminder(BigInt* a, BigInt* b);
void bigint_div_one_into_reminder(BigInt* a, arrtype bintb);
BigInt* bigint_div_into_return_reminder(BigInt* a, BigInt* b);
BigInt* bigint_div_one_into_return_reminder(BigInt* a, arrtype bintb);

// a^b, b > 0
BigInt* bigint_power(BigInt* a, BigInt* b);
void bigint_power_into(BigInt* a, BigInt* b);
BigInt* bigint_power_one(BigInt* a, arrtype bintb);
void bigint_power_one_into(BigInt* a, arrtype bintb);
char* pwd_dec_integer(char* sbinta, char* sbintb);
char* pwd_dec_integer_one(char* sbinta, arrtype bintb);

// sqrt(n)
BigInt* bigint_sqrt(BigInt* bigint);
void bigint_sqrt_into(BigInt* bigint);
char* sqrt_dec_integer(char* decstr);

// n!
BigInt* bigint_factor(arrtype n);
char* factor_return_dec_string(arrtype n);

// C^N_M
BigInt* bigint_combination(arrtype n, arrtype m);


#endif // _BIG_INT_H_
