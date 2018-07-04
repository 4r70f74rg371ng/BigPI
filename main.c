#include <stdio.h>
#include <stdlib.h>
#include "BigInt.h"

void get_pi(int digit){
	BigInt* base = NULL;
	BigInt* bdigit = NULL;
	BigInt* total = NULL;
	BigInt* a = NULL;
	BigInt* b = NULL;
	BigInt* c = NULL;
	BigInt* d = NULL;
	char* snum = NULL;
	
	int i;
	
	bdigit = bigint_initial_ten_base_one(1, digit);
	base = bigint_initial_one(1);
	total = bigint_initial_one(0);
	
	for(i=0;i<=10000;i++){
		a = bigint_mul_one(bdigit, 4);
		b = bigint_mul_one(bdigit, 2);
		c = bigint_duplicate(bdigit);
		d = bigint_duplicate(bdigit);
		
		bigint_div_one_into(a, 8*i+1);
		bigint_div_one_into(b, 8*i+4);
		bigint_div_one_into(c, 8*i+5);
		bigint_div_one_into(d, 8*i+6);
		
		bigint_del_into(a, b);
		bigint_del_into(a, c);
		bigint_del_into(a, d);
		
		bigint_div_into(a, base);
		
		bigint_mul_one_into(base, 16);
		
		bigint_add_into(total, a);
		
		bigint_cleanup_pointer(&a);
		bigint_cleanup_pointer(&b);
		bigint_cleanup_pointer(&c);
		bigint_cleanup_pointer(&d);
	}
	
	snum = bigint_print_return_string(total);
	printf("'%s'\n", snum);
	free(snum);
	
	bigint_cleanup_pointer(&bdigit);
	bigint_cleanup_pointer(&base);
	bigint_cleanup_pointer(&total);
}

int main(int argc, char *argv[]) {
	
	get_pi(100);
	
	/*int i;
	BigInt* num = NULL;
	char* snum;
	num = bigint_initial_one(1);
	
	for(i=0;i<1000;i++){
		bigint_mul_one_into(num, i+1);
	}
	snum = bigint_print_return_string(num);
	bigint_cleanup(num);
	printf("%s\n", snum);
	free(snum);*/
	
	return 0;
}
