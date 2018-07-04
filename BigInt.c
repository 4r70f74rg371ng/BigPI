#include "BigInt.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <math.h>

BigInt* bigint_initial_dec_string(char* sinteger) {
	BigInt* ret = NULL;
	arrtype i;
	arrtype len = 0, tmp;
	arrtype ret_len = 0;
	arrtype offset = 0;
	const arrtype digital = __bigint_digital;
	const arrtype base = __bigint_base;
	char* handled_string = NULL;

	if (sinteger != NULL) {
		// base integer base = 10^digital
		/*for (i = 0; i < digital; i++) {
		base *= 10;
		}*/
		// basic confirm if base integer is not overflow
		//if ((base > 0) && (base % 10 == 0)) {
		// get length of string
		for (len = 0; sinteger[len]; len++);

		handled_string = (char*)malloc(sizeof(char)*(len + 1));

		if (handled_string != NULL) {

			// got handled string
			for (tmp = i = 0; i < len; i++) {
				if ((sinteger[i] >= '0') && (sinteger[i] <= '9') || (sinteger[i] == '-')) {
					handled_string[tmp++] = sinteger[i];
				}
			}
			// length recount
			len = tmp;

			if (handled_string[0] == '-') {
				ret_len = (len + digital - 2) / digital;
			}
			else {
				ret_len = (len + digital - 1) / digital;
			}

			ret = (BigInt*)malloc(sizeof(BigInt));

			if (ret != NULL) {
				ret->arrlen = ret_len;
				ret->base = base;
				ret->digital = digital;
				ret->sign = 0;

				if (handled_string[0] == '-') {
					ret->sign = 1;
					offset = 1;
				}

				ret->arr = (arrtype*)malloc(sizeof(arrtype)*(ret_len));

				if (ret->arr != NULL) {
					for (i = 0; i < ret_len; i++) {
						ret->arr[i] = 0;
					}
					// convert string to big integer
					tmp = 1;
					for (i = offset; i < len; i++) {
						if (handled_string[i] >= '0' && handled_string[i] <= '9') {
							if (((i - offset) % digital) == 0) {
								tmp = 1;
							}
							ret->arr[(i - offset) / digital] = ret->arr[(i - offset) / digital] + (handled_string[len - i - 1 + offset] - '0') * tmp;
							tmp *= 10;
						}
					}
					if ((offset == (len - 1)) && (ret->arr[0] == 0)) { //todo:check
						ret->sign = 0;
					}
				}
				else {
					free(ret);
					ret = NULL;
				}
			}
			free(handled_string);
			//}
		}
	}
	return ret;
}

BigInt* bigint_initial_from_array(arrtype sign, arrtype* arr, arrtype size) {
	BigInt* ret = NULL;
	int i;

	if (arr) {
		ret = (BigInt*)malloc(sizeof(BigInt));
		if (ret) {
			ret->arr = (arrtype*)malloc(sizeof(arrtype)*(size));
			if (ret->arr) {
				ret->sign = sign;
				ret->arrlen = size;
				ret->base = __bigint_base;
				ret->digital = __bigint_digital;

				for (i = 0; i < size; i++) {
					ret->arr[i] = arr[i];
				}
			}
			else {
				free(ret);
				ret = NULL;
			}
		}
	}
	return ret;
}

BigInt* bigint_initial_zero() {
	BigInt* ret = NULL;
	const arrtype digital = __bigint_digital;
	const arrtype base = __bigint_base;

	ret = (BigInt*)malloc(sizeof(BigInt));
	if (ret != NULL) {
		ret->base = base;
		ret->digital = digital;
		ret->sign = 0;
		ret->arrlen = 1;
		ret->arr = (arrtype*)malloc(sizeof(arrtype));
		if (ret->arr) {
			ret->arr[0] = 0;
		}
		else {
			ret->arrlen = 0;
		}
	}

	return ret;
}

BigInt* bigint_initial_size(arrtype size) {
	BigInt* ret = NULL;
	const arrtype digital = __bigint_digital;
	const arrtype base = __bigint_base;
	arrtype i;

	ret = (BigInt*)malloc(sizeof(BigInt));
	if (ret != NULL) {
		ret->base = base;
		ret->digital = digital;
		ret->sign = 0;
		ret->arrlen = size;
		ret->arr = (arrtype*)malloc(sizeof(arrtype)*size);
		if (ret->arr) {
			for (i = 0; i < size; i++) {
				ret->arr[i] = 0;
			}
		}
		else {
			ret->arrlen = 0;
		}
	}

	return ret;
}

// safe version's base
BOOL bigint_isneed_normalize(BigInt* bint) {
	arrtype i;
	arrtype first;
	if (__bigint_magic_assert(bint)) {

		// test zero adjust
		if ((bint->sign == 1) && bigint_iszero(bint)) {
			return TRUE;
		}

		// caculate first value
		first = 0;
		for (i = bint->arrlen - 1; i >= 0; i--) {
			if (bint->arr[i] != 0) {
				break;
			}
		}
		if (i >= 0) {
			first = i + 1;
		}
		else {
			first = 1;
		}

		// doing sign trasform
		if (bint->arr[first - 1] < 0) {
			return TRUE;
		}

		// borrow
		for (i = 0; i < (first - 1); i++) {
			if (bint->arr[i] < 0) {
				return TRUE;
			}
		}

		// carry
		for (i = first - 2; i >= 0; i--) {
			if (bint->arr[i] >= bint->base) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

int bigint_iszero(BigInt* bigint) {
	arrtype i;

	if (__bigint_magic_assert(bigint)) {

		if (bigint->arrlen <= 0) {
			return 1;
		}

		if (bigint->arr == NULL) {
			return 1;
		}

		for (i = bigint->arrlen - 1; i >= 0; i--) {
			if (bigint->arr[i] != 0) {
				break;
			}
		}

		if (i < 0) {
			return 1;
		}
		return 0;
	}
	return 1;
}

// a != 0, b != 0, a->sign == b->sign
int bigint_compare_internal(BigInt* a, BigInt* b, arrtype afirst, arrtype bfirst) {
	arrtype i;
	if (__bigint_magic_assert(a) && __bigint_magic_assert(b) && (a->sign == b->sign) && (afirst >= 0) && (bfirst >= 0)) {
		if (afirst > bfirst) {
			return 1;
		}
		else if (afirst < bfirst) {
			return -1;
		}
		else {
			for (i = afirst; i >= 0; i--) {
				if (a->arr[i] > b->arr[i]) {
					return 1;
				}
				else if (a->arr[i] < b->arr[i]) {
					return -1;
				}
			}
		}
	}
	return 0;
}

// <0: compare(a,b) = sign(a-b) = (a-b)/|(a-b)|
int bigint_compare(BigInt* a, BigInt* b) {
	arrtype afirst, bfirst;
	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {
		for (afirst = a->arrlen - 1; afirst >= 0; afirst--) {
			if (a->arr[afirst] != 0) {
				break;
			}
		}
		for (bfirst = b->arrlen - 1; bfirst >= 0; bfirst--) {
			if (b->arr[bfirst] != 0) {
				break;
			}
		}
		if (afirst < 0) {
			if (bfirst < 0) {
				return 0;
			}
			else if (b->sign == 0) {
				return -1;
			}
			else {
				return 1;
			}
		}
		else if (a->sign == 0) {
			if (bfirst < 0) {
				return 1;
			}
			else if (b->sign == 0) {
				return bigint_compare_internal(a, b, afirst, bfirst);
			}
			else {
				return 1;
			}
		}
		else {
			if (bfirst < 0) {
				return -1;
			}
			else if (b->sign == 0) {
				return -1;
			}
			else {
				return -bigint_compare_internal(a, b, afirst, bfirst);
			}
		}
	}
	return 0;
}

int bigint_compare_one(BigInt* a, arrtype bintb) {
	BigInt* num = NULL;
	int iresult = 0;
	if (__bigint_magic_assert(a)) {
		num = bigint_initial_one(bintb);
		iresult = bigint_compare(a, num);
	}
	return iresult;
}

void bigint_print_info(BigInt* bigint) {
	arrtype i;
	char* spointer = NULL;
	if (bigint) {
		printf("BigInt instance's memory address: %p\n", bigint);
		printf("BigInt's base: %llu\n", bigint->base);
		printf("BigInt's digital of one integer: %llu\n", bigint->digital);
		printf("BigInt->sign is sign(positive[+]/negtive[-]) of integer: %c (%lld)\n", bigint->sign ? '-' : '+', bigint->sign);
		printf("BigInt->arrlen is length of array of integer: %llu\n", bigint->arrlen);
		printf("BigInt->arr is array of integer: %p = {", bigint->arr);
		for (i = 0; i < bigint->arrlen; i++) {
			if (i != 0) {
				printf(", ");
			}
			printf("[%llu] = %llu", i, bigint->arr[i]);
		}
		printf("}\n");

		spointer = bigint_print_return_string(bigint);
		printf("BigInt representation is: '%s'\n", spointer);
		free(spointer);
	}
}

char* bigint_print_return_string(BigInt* bigint) {
	arrtype i, j;
	char* ret = NULL;
	char* tmp = NULL;
	if (__bigint_magic_assert(bigint)) {

		if (bigint->sign == 2) {
			ret = (char*)malloc(sizeof(char)*(10));
			sprintf(ret, "inf");
		}
		else if (bigint->sign == -2) {
			ret = (char*)malloc(sizeof(char)*(10));
			sprintf(ret, "-inf");
		}
		else {

			ret = (char*)malloc(sizeof(char)*(bigint->arrlen*bigint->digital + 2));
			tmp = (char*)malloc(sizeof(char)*(bigint->digital * 2 + 2));

			if ((ret != NULL) && (tmp != NULL)) {
				ret[0] = '\0';
				if (bigint->sign == 1) {
					sprintf(tmp, "-");
					strcat(ret, tmp);
				}
				for (i = bigint->arrlen - 1; i >= 0; i--) {
					if (bigint->arr[i] != 0) {
						break;
					}
				}
				if (i < 0) {
					strcpy(ret, "0");
				}
				else {
					sprintf(tmp, "%lld", bigint->arr[i]);
					strcat(ret, tmp);
					for (j = i - 1; j >= 0; j--) {
						sprintf(tmp, "%0*lld", (int)bigint->digital, bigint->arr[j]);
						strcat(ret, tmp);
					}
				}
			}

			if (tmp != NULL) {
				free(tmp);
				tmp = NULL;
			}
		}
	}

	return ret;
}

void bigint_print(BigInt* bigint) {
	char* str = NULL;
	if (__bigint_magic_assert(bigint)) {
		str = bigint_print_return_string(bigint);
		if (str) {
			printf("%s", str);
			free(str);
		}
	}
}

void bigint_cleanup(BigInt* bigint) {
	if (__bigint_magic_assert(bigint)) {
		if (bigint->arr) {
			free(bigint->arr);
		}
		bigint->arr = NULL;
		bigint->arrlen = 0;
		bigint->base = 0;
		bigint->digital = 0;
		bigint->sign = 0;
	}
}

void bigint_cleanup_pointer(BigInt** bigint) {
	if (bigint && __bigint_magic_assert(*bigint)) {
		if ((*bigint)->arr) {
			free((*bigint)->arr);
		}
		(*bigint)->arr = NULL;
		(*bigint)->arrlen = 0;
		(*bigint)->base = 0;
		(*bigint)->digital = 0;
		(*bigint)->sign = 0;
		free(*bigint);
		*bigint = NULL;
	}
}

// not handle in sign
// special setting: a->sign == b->sign
BigInt* bigint_add_return_internal(BigInt* a, BigInt* b) {
	arrtype* result = NULL;
	arrtype* result_tmp = NULL;
	arrtype result_length = 0;
	arrtype real_length = 0;
	arrtype i;
	arrtype expanding[2] = { 0 };
	arrtype expanding_top = 0;
	BigInt* ret = NULL;

	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {

		// assert some setting is right & same
		assert(a->base == b->base);
		assert(a->digital == b->digital);
		assert(a->sign == b->sign);

		// pre-caculate result array length
		result_length = a->arrlen;
		if (result_length < b->arrlen) {
			result_length = b->arrlen;
		}
		result_length++; // for carry space

		result = (arrtype*)malloc(sizeof(arrtype)*result_length);
		ret = (BigInt*)malloc(sizeof(BigInt));

		if ((result != NULL) && (ret != NULL)) {
			// clean array
			for (i = 0; i < result_length; i++) {
				result[i] = 0;
			}

			// copy from a
			for (i = 0; i < a->arrlen; i++) {
				result[i] = a->arr[i];
			}

			// add from b
			for (i = 0; i < b->arrlen; i++) {
				result[i] += b->arr[i];
			}

			// do carry
			for (i = 0; i < result_length - 1; i++) {
				result[i + 1] += result[i] / a->base;
				result[i] = result[i] % a->base;
			}

			// do expanding if need
			if (result[result_length - 1] > a->base) {
				result_tmp = (arrtype*)malloc(sizeof(arrtype)*(result_length + 2));
				if (result_tmp) {

					expanding[0] = result[result_length - 1] / a->base;
					result[result_length - 1] = result[result_length - 1] % a->base;
					expanding[1] = expanding[0] / a->base;

					if ((expanding[1] != 0) || (expanding[0] != 0)) {
						// get expanding length
						if (expanding[1] != 0) {
							expanding_top = 1;
						}
						else {
							expanding_top = 0;
						}

						for (i = 0; i < result_length; i++) {
							result_tmp[i] = result[i];
						}
						result_tmp[result_length] = expanding[0];
						result_tmp[result_length + 1] = expanding[1];

						free(result);
						result = result_tmp;
						result_tmp = NULL;
						result_length += 2;
					}
				}
			}

			// do compress
			for (real_length = result_length - 1; (real_length >= 0) && (result[real_length] == 0); real_length--);
			real_length++;

			if (real_length <= 0) {
				real_length = 1;
			}

			result_tmp = (arrtype*)malloc(sizeof(arrtype)*(real_length));

			if (result_tmp != NULL) {

				for (i = 0; i < real_length; i++) {
					result_tmp[i] = result[i];
				}

				free(result);
				result = NULL;

				// fill in data structure
				ret->arr = result_tmp;
				ret->arrlen = real_length;
				ret->base = a->base;
				ret->digital = a->digital;

				// nothing right if 
				ret->sign = a->sign;
			}
			else {
				// fill in data structure
				ret->arr = result;
				ret->arrlen = result_length;
				ret->base = a->base;
				ret->digital = a->digital;

				// nothing right if 
				ret->sign = a->sign;
			}
		}
		else {
			if (result != NULL) {
				free(result);
				ret = NULL;
			}
			if (ret != NULL) {
				free(ret);
				ret = NULL;
			}
		}
	}
	return ret;
}

BigInt* bigint_del_return_internal(BigInt* a, BigInt* b) {
	arrtype* result = NULL;
	arrtype* result_tmp = NULL;
	arrtype result_length = 0;
	arrtype real_length = 0;
	arrtype sign = 0;
	arrtype i;
	BigInt* ret = NULL;

	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {
		// assert some setting is right & same
		assert(a->base == b->base);
		assert(a->digital == b->digital);

		// pre-caculate result array length
		result_length = a->arrlen;
		if (result_length < b->arrlen) {
			result_length = b->arrlen;
		}

		result = (arrtype*)malloc(sizeof(arrtype)*result_length);
		ret = (BigInt*)malloc(sizeof(BigInt));

		if ((result != NULL) && (ret != NULL)) {
			// clean array
			for (i = 0; i < result_length; i++) {
				result[i] = 0;
			}

			// copy from a
			for (i = 0; i < a->arrlen; i++) {
				result[i] = a->arr[i];
			}

			// del from b
			for (i = 0; i < b->arrlen; i++) {
				result[i] -= b->arr[i];
			}

			// get top
			for (real_length = result_length - 1; (real_length >= 0) && (result[real_length] == 0); real_length--);
			real_length++;

			if (real_length == 0) {
				real_length++;
			}

			// negtive trasform
			sign = 0;
			if (result[real_length - 1] < 0) {
				sign = 1;
				for (i = 0; i < real_length; i++) {
					result[i] = -result[i];
				}
			}

			// borrow
			for (i = real_length - 2; i >= 0; i--) {
				if (result[i] < 0) {
					result[i + 1]--;
					result[i] += a->base;
				}
			}

			// do compress
			result_tmp = (arrtype*)malloc(sizeof(arrtype)*(real_length));
			if (result_tmp != NULL) {
				for (i = 0; i < real_length; i++) {
					result_tmp[i] = result[i];
				}

				free(result);
				result = result_tmp;
				result_tmp = NULL;
			}

			// fill data structure
			ret->arr = result;
			ret->arrlen = real_length;
			ret->base = a->base;
			ret->digital = a->digital;
			ret->sign = sign;
		}
		else {
			if (result != NULL) {
				free(result);
				ret = NULL;
			}
			if (ret != NULL) {
				free(ret);
				ret = NULL;
			}
		}
	}

	return ret;
}

BigInt* bigint_add(BigInt* a, BigInt* b) {
	arrtype signa, signb;
	BigInt* c = NULL;

	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {
		// sign backup
		signa = a->sign;
		signb = b->sign;

		a->sign = b->sign = 0;
		if (signa^signb) {
			c = bigint_del_return_internal(a, b);
			if (signa == 1) {
				c->sign = 1 - c->sign; // flip sign
			}
		}
		else {
			c = bigint_add_return_internal(a, b);
			if (signa == 1) {
				c->sign = 1 - c->sign; // flip sign
			}
		}

		//sign recovery
		a->sign = signa;
		b->sign = signb;

		// zero adjust bug
		if ((c->sign == 1) && bigint_iszero(c)) {
			c->sign = 0;
		}
	}
	return c;
}

BigInt* bigint_del(BigInt* a, BigInt* b) {
	arrtype signa, signb;
	BigInt* c = NULL;

	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {
		// sign backup
		signa = a->sign;
		signb = b->sign;

		a->sign = b->sign = 0;
		if (signa^signb) {
			c = bigint_add_return_internal(a, b);
			if (signa == 1) {
				c->sign = 1 - c->sign; // flip sign
			}
		}
		else {
			c = bigint_del_return_internal(a, b);
			if (signa == 1) {
				c->sign = 1 - c->sign; // flip sign
			}
		}

		//sign recovery
		a->sign = signa;
		b->sign = signb;

		// zero adjust bug
		if ((c->sign == 1) && bigint_iszero(c)) {
			c->sign = 0;
		}
	}
	return c;
}

char* add_dec_integer(char* sbinta, char* sbintb) {
	BigInt* binta = NULL;
	BigInt* bintb = NULL;
	BigInt* bintc = NULL;

	char* ret = NULL;

	if (sbinta && sbintb) {
		binta = bigint_initial_dec_string(sbinta);
		if (binta != NULL) {
			bintb = bigint_initial_dec_string(sbintb);
			if (bintb != NULL) {
				bintc = bigint_add(binta, bintb);
				if (bintc != NULL) {
					ret = bigint_print_return_string(bintc);
					bigint_cleanup(bintc);
				}
				bigint_cleanup(bintb);
			}
			bigint_cleanup(binta);
		}
	}

	return ret;
}

char* del_dec_integer(char* sbinta, char* sbintb) {
	BigInt* binta = NULL;
	BigInt* bintb = NULL;
	BigInt* bintc = NULL;

	char* ret = NULL;

	if (sbinta && sbintb) {
		binta = bigint_initial_dec_string(sbinta);
		if (binta != NULL) {
			bintb = bigint_initial_dec_string(sbintb);
			if (bintb != NULL) {
				bintc = bigint_del(binta, bintb);
				if (bintc != NULL) {
					ret = bigint_print_return_string(bintc);
					bigint_cleanup(bintc);
				}
				bigint_cleanup(bintb);
			}
			bigint_cleanup(binta);
		}

	}

	return ret;
}

// (+) + (+)
BigInt* bigint_add_one_internal(BigInt* a, arrtype b) {
	BigInt* bint = NULL;
	arrtype barr[3];
	arrtype *arr, *tmp;
	arrtype i;
	arrtype real_length = 0;
	if (__bigint_magic_assert(a)) {
		barr[0] = b % a->base;
		barr[1] = (b / a->base) % a->base;
		barr[2] = b / (a->base * a->base);

		arr = (arrtype*)malloc(sizeof(arrtype)*(a->arrlen + 3));
		bint = (BigInt*)malloc(sizeof(BigInt));
		if ((arr != NULL) && (bint != NULL)) {
			for (i = 0; i < (a->arrlen + 3); i++) {
				arr[i] = 0;
			}

			for (i = 0; i < a->arrlen; i++) {
				arr[i] = a->arr[i];
			}

			for (i = 0; i < 3; i++) {
				arr[i] += barr[i];
			}

			for (i = 0; i < (a->arrlen + 2); i++) {
				arr[i + 1] += (arr[i] / a->base);
				arr[i] = arr[i] % a->base;
			}

			for (real_length = (a->arrlen + 2); (real_length >= 0) && (arr[real_length] == 0); real_length--);
			real_length++;

			if (real_length <= 0) {
				real_length = 1;
			}

			tmp = (arrtype*)malloc(sizeof(arrtype)*(real_length));

			if (tmp) {
				for (i = 0; i < real_length; i++) {
					tmp[i] = arr[i];
				}
				free(arr);
				arr = NULL;

				bint->arr = tmp;
				bint->arrlen = real_length;
				bint->base = a->base;
				bint->digital = a->digital;
				bint->sign = a->sign;
			}
			else {
				bint->arr = arr;
				bint->arrlen = a->arrlen + 3;
				bint->base = a->base;
				bint->digital = a->digital;
				bint->sign = a->sign;
			}
		}
		else {
			if (arr != NULL) {
				free(arr);
			}

			if (bint != NULL) {
				free(bint);
				bint = NULL;
			}
		}
	}
	return bint;
}

// (+) - (+) or (-) + (-)
BigInt* bigint_del_one_internal(BigInt* a, arrtype b) {
	BigInt* bint = NULL;
	arrtype barr[3];
	arrtype *arr, *tmp;
	arrtype i;
	arrtype real_length = 0;
	if (__bigint_magic_assert(a)) {
		barr[0] = b % a->base;
		barr[1] = (b / a->base) % a->base;
		barr[2] = b / (a->base * a->base);

		arr = (arrtype*)malloc(sizeof(arrtype)*(a->arrlen + 3));
		bint = (BigInt*)malloc(sizeof(BigInt));
		if ((arr != NULL) && (bint != NULL)) {
			for (i = 0; i < (a->arrlen + 3); i++) {
				arr[i] = 0;
			}

			for (i = 0; i < a->arrlen; i++) {
				arr[i] = a->arr[i];
			}

			for (i = 0; i < 3; i++) {
				arr[i] -= barr[i];
			}

			// TODO: do borrow => get head first

			for (real_length = (a->arrlen + 2); (real_length >= 0) && (arr[real_length] == 0); real_length--);
			real_length++;

			if (real_length <= 0) {
				real_length = 1;
			}

			// 1. sign transform
			if (arr[real_length - 1] < 0) {
				a->sign = 1 - a->sign;
				arr[real_length - 1] *= -1;
			}

			// 2. borrow
			// [a][b][c]
			//  ------->
			// [a-b/B][b%B][c]
			// borrow [-(base-1)...(base-1)] 
			// if [-(base-1)... -1] => [-1][1...(base-1)]
			for (i = 0; i < (real_length - 1); i++) {
				if (arr[i] < 0) {
					arr[i + 1] --;
					arr[i] += a->base;
				}
			}


			// 3. re-adjust length
			for (; (real_length >= 0) && (arr[real_length] != 0); real_length--);
			real_length++;

			if (real_length <= 0) {
				real_length = 1;
			}

			tmp = (arrtype*)malloc(sizeof(arrtype)*(real_length));

			if (tmp) {
				for (i = 0; i < real_length; i++) {
					tmp[i] = arr[i];
				}
				free(arr);
				arr = NULL;

				bint->arr = tmp;
				bint->arrlen = real_length;
				bint->base = a->base;
				bint->digital = a->digital;
				bint->sign = a->sign;
			}
			else {
				bint->arr = arr;
				bint->arrlen = a->arrlen + 3;
				bint->base = a->base;
				bint->digital = a->digital;
				bint->sign = a->sign;
			}
		}
		else {
			if (arr != NULL) {
				free(arr);
			}

			if (bint != NULL) {
				free(bint);
				bint = NULL;
			}
		}
	}
	return bint;
}

BigInt* bigint_add_one(BigInt* a, arrtype b) {
	arrtype signa, signb;
	BigInt* ret = NULL;

	if (__bigint_magic_assert(a)) {
		signa = a->sign;
		signb = (b < 0) ? 1 : 0;

		a->sign = 0;

		if (signa == signb) {
			ret = bigint_add_one_internal(a, signb ? -b : b);
			ret->sign = signa;
		}
		else {
			// +, - => (|a|-|b|)
			// -, + => -(|a|-|b|)
			ret = bigint_del_one_internal(a, signb ? -b : b);
			ret->sign = signa ^ ret->sign;
		}

		// recovery sign
		a->sign = signa;

		// zero adjust bug
		if ((ret->sign == 1) && bigint_iszero(ret)) {
			ret->sign = 0;
		}
	}

	return ret;
}

BigInt* bigint_del_one(BigInt* a, arrtype b) {
	arrtype signa, signb;
	BigInt* ret = NULL;

	if (__bigint_magic_assert(a)) {
		signa = a->sign;
		signb = (b < 0) ? 1 : 0;

		a->sign = 0;

		if (signa == signb) {
			ret = bigint_del_one_internal(a, signb ? -b : b);
			ret->sign = ret->sign ^ signa;
		}
		else {
			ret = bigint_add_one_internal(a, signb ? -b : b);
			ret->sign = signa;
		}

		// recovery sign
		a->sign = signa;

		// zero adjust bug
		if ((ret->sign == 1) && bigint_iszero(ret)) {
			ret->sign = 0;
		}
	}

	return ret;
}

/*
|a| * |b|
= |signa*(a0*base^n+a1^base^(n-1)+...+an)| * |signb*(b0*base^m+b1^base^(m-1)+...+bm)|
= (a0*base^n+a1^base^(n-1)+...+an) * (b0*base^m+b1^base^(m-1)+...+bm)
= |signa*(a0 a1 ... an)| * |signb*(b0 b1 ... bm)|
= (a0 a1 ... an) * (b0 b1 ... bm)

sigma_i(sigma_j(ai*bj*base^(i+j)))
sigma_{(i+j)==k}{sigma_k(a_i+a_j)}

*/
BigInt* bigint_mul(BigInt* a, BigInt* b) {
	BigInt* ret = NULL;
	arrtype * tmp = NULL;
	arrtype tmplen = 0;
	arrtype i, j;
	arrtype value, carry;
	// arrtype tmpvalue[2] = {0};

	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {

		ret = (BigInt*)malloc(sizeof(BigInt));

		if (ret != NULL) {
			ret->base = a->base;
			ret->digital = a->digital;
			ret->sign = a->sign ^ b->sign;

			// 1. allocate temperary array for multiply big integer result
			tmplen = a->arrlen + b->arrlen + 1;
			tmp = (arrtype*)malloc(sizeof(arrtype)*(tmplen));

			// 2. clean tmp array
			for (i = 0; i < tmplen; i++) {
				tmp[i] = 0;
			}

			// 3. do multiply
			for (i = a->arrlen - 1; i >= 0; i--) {
				carry = 0;
				// a * b
				for (j = 0; j < b->arrlen; j++) {
					value = tmp[i + j] + a->arr[i] * b->arr[j] + carry;
					/*
					// overflow prevension => no need to do MIN_INT64 < base*base+base*2 <  MAX_INT64
					tmpvalue[0] = a->arr[i] * b->arr[j];
					if (tmpvalue[0] >= a->base) {
					tmpvalue[1] = tmpvalue[0] / a->base;
					tmpvalue[0] %= a->base;
					}

					tmpvalue[0] += tmp[i + j];
					if (tmpvalue[0] >= a->base) {
					tmpvalue[1] = tmpvalue[0] / a->base;
					tmpvalue[0] %= a->base;
					}

					tmpvalue[0] += carry;
					if (tmpvalue[0] >= a->base) {
					tmpvalue[1] = tmpvalue[0] / a->base;
					tmpvalue[0] %= a->base;
					}*/

					tmp[i + j] = value % a->base;
					carry = value / a->base;
				}

				// reminder carry
				if (carry != 0) {
					tmp[i + b->arrlen] += carry;
					for (j = 0; tmp[i + b->arrlen + j] >= a->base; j++) {
						tmp[i + b->arrlen + j + 1] += tmp[i + b->arrlen + j] / a->base;
						tmp[i + b->arrlen + j] %= a->base;
					}
				}
			}

			// 4. do compress => find head
			for (i = tmplen - 1; i >= 0; i--) {
				if (tmp[i] != 0) {
					break;
				}
			}
			// first == i
			if (i < 0) {
				ret->arrlen = 1;
				ret->arr = (arrtype*)malloc(sizeof(arrtype));
				if (ret->arr) {
					ret->arr[0] = 0;
				}
				else {
					ret->arrlen = 0;
				}
			}
			else {
				ret->arrlen = i + 1;
				ret->arr = (arrtype*)malloc(sizeof(arrtype)*(i + 1));
				if (ret->arr != NULL) {
					for (j = 0; j <= i; j++) {
						ret->arr[j] = tmp[j];
					}
				}
				free(tmp);
			}

			// zero adjust bug
			if ((ret->sign == 1) && bigint_iszero(ret)) {
				ret->sign = 0;
			}
		} // if ret
	} // if a && b
	return ret;
}

char* mul_dec_integer(char* sbinta, char* sbintb) {
	BigInt* binta = NULL;
	BigInt* bintb = NULL;
	BigInt* bintc = NULL;

	char* ret = NULL;

	if (sbinta && sbintb) {
		binta = bigint_initial_dec_string(sbinta);
		if (binta != NULL) {
			bintb = bigint_initial_dec_string(sbintb);
			if (bintb != NULL) {
				bintc = bigint_mul(binta, bintb);
				if (bintc != NULL) {
					ret = bigint_print_return_string(bintc);
					bigint_cleanup(bintc);
				}
				bigint_cleanup(bintb);
			}
			bigint_cleanup(binta);
		}

	}

	return ret;
}

void bigint_normalize(BigInt* bint) {
	arrtype* tmp = NULL, *tmparr = NULL;
	arrtype sign;
	arrtype i;
	arrtype first;
	arrtype oldfirst = -1;
	int changed = 0;
	if (__bigint_magic_assert(bint)) {

		if ((bint->sign == 1) && (bint->arr[0] == 0) && (bigint_iszero(bint))) {
			bint->sign = 0;
		}

		// allocation space for normalization
		tmp = (arrtype*)malloc(sizeof(arrtype)*(bint->arrlen + 4)); // for allocation

		if (tmp != NULL) {
			// copy values from bint to tmp array
			for (i = 0; i < bint->arrlen + 4; i++) {
				if (i < bint->arrlen) {
					tmp[i] = bint->arr[i];
				}
				else {
					tmp[i] = 0;
				}
			}

			sign = bint->sign;

			do {
				changed = 0;

				// caculate first value
				first = 0;
				for (i = bint->arrlen - 1; i >= 0; i--) {
					if (tmp[i] != 0) {
						break;
					}
				}
				if (i >= 0) {
					first = i + 1;
				}
				else {
					first = 1;
				}

				if (oldfirst == -1) {
					oldfirst = first;
					changed = 1;
				}
				else if (oldfirst != first) {
					oldfirst = first;
					changed = 1;
				}

				// doing sign trasform
				if (tmp[first - 1] < 0) {
					sign = 1 - sign;
					for (i = 0; i < first; i++) {
						tmp[i] = -tmp[i];
					}
					changed = 1;
				}

				// borrow
				for (i = 0; i <= (first - 1); i++) {
					if (tmp[i] < 0) {
						tmp[i + 1] -= (bint->base - tmp[i] - 1) / bint->base;
						tmp[i] = bint->base - ((-tmp[i]) % bint->base);
						changed = 1;
					}
				}
			} while (changed);

			// carry
			for (i = first - 2; i >= 0; i--) {
				tmp[i + 1] += tmp[i] / bint->base;
				tmp[i] += tmp[i] % bint->base;
			}
			
			tmparr = (arrtype*)malloc(sizeof(arrtype)*(first + 1));

			if (tmparr != NULL) {
				free(bint->arr);
				bint->sign = sign;
				bint->arr = tmparr;
				for (i = 0; i <= first; i++) {
					bint->arr[i] = tmp[i];
				}

				bint->arrlen = first + 1;
			}
			else {

			}
			free(tmp);
		}
	}
}

char* add_dec_integer_one(char* sbinta, arrtype bintb) {
	BigInt* a = NULL;
	BigInt* b = NULL;
	char* ret = NULL;

	if (sbinta != NULL) {
		a = bigint_initial_dec_string(sbinta);
		if (__bigint_magic_assert(a)) {
			b = bigint_add_one(a, bintb);
			if (__bigint_magic_assert(b)) {
				ret = bigint_print_return_string(b);
				bigint_cleanup_pointer(&b);
			}
			bigint_cleanup_pointer(&a);
		}
	}

	return  ret;
}

char* del_dec_integer_one(char* sbinta, arrtype bintb) {
	BigInt* a = NULL;
	BigInt* b = NULL;
	char* ret = NULL;

	if (sbinta != NULL) {
		a = bigint_initial_dec_string(sbinta);
		if (__bigint_magic_assert(a)) {
			b = bigint_del_one(a, bintb);
			if (__bigint_magic_assert(b)) {
				ret = bigint_print_return_string(b);
				bigint_cleanup_pointer(&b);
			}
			bigint_cleanup_pointer(&a);
		}
	}

	return  ret;
}

BigInt* bigint_mul_one(BigInt* a, arrtype b) {
	BigInt* ret = NULL;
	arrtype * tmp = NULL;
	arrtype tmplen = 0;
	arrtype i, j;
	arrtype value, carry;
	arrtype bsign = 0;
	arrtype tmpvalue[3] = { 0 };

	if (__bigint_magic_assert(a)) {

		ret = (BigInt*)malloc(sizeof(BigInt));

		if (ret != NULL) {

			// b => tmpvalue[3], bsign
			bsign = (b < 0) ? 1 : 0;
			tmpvalue[0] = (b < 0) ? -b : b;

			tmpvalue[2] = tmpvalue[0] / (a->base*a->base);
			tmpvalue[1] = (tmpvalue[0] / (a->base)) % a->base;
			tmpvalue[0] = tmpvalue[0] % a->base;

			// initial struct value
			ret->base = a->base;
			ret->digital = a->digital;
			ret->sign = a->sign ^ bsign;

			// do |a| * |b|

			// 1. allocate temperary array for multiply big integer result
			tmplen = a->arrlen + 3 + 1;
			tmp = (arrtype*)malloc(sizeof(arrtype)*(tmplen));
			if (tmp != NULL) {

				// 2. clean tmp array
				for (i = 0; i < tmplen; i++) {
					tmp[i] = 0;
				}

				// 3. do multiply
				for (i = a->arrlen - 1; i >= 0; i--) {
					carry = 0;
					// a * b
					for (j = 0; j < 3; j++) {
						value = tmp[i + j] + a->arr[i] * tmpvalue[j] + carry;

						tmp[i + j] = value % a->base;
						carry = value / a->base;
					}

					// reminder carry
					if (carry != 0) {
						tmp[i + 3] += carry;
						for (j = 0; tmp[i + 3 + j] >= a->base; j++) {
							tmp[i + 3 + j + 1] += tmp[i + 3 + j] / a->base;
							tmp[i + 3 + j] %= a->base;
						}
					}
				}

				// 4. do compress => find head
				for (i = tmplen - 1; i >= 0; i--) {
					if (tmp[i] != 0) {
						break;
					}
				}
				// first == i
				if (i < 0) {
					ret->arrlen = 1;
					ret->arr = (arrtype*)malloc(sizeof(arrtype));
					if (ret->arr) {
						ret->arr[0] = 0;
					}
					else {
						ret->arrlen = 0;
					}
				}
				else {
					ret->arrlen = i + 1;
					ret->arr = (arrtype*)malloc(sizeof(arrtype)*(i + 1));
					if (ret->arr != NULL) {
						for (j = 0; j <= i; j++) {
							ret->arr[j] = tmp[j];
						}
					}
					free(tmp);
				}

				// zero adjust bug
				if ((ret->sign == 1) && bigint_iszero(ret)) {
					ret->sign = 0;
				}
			}
			else {  // if tmp
				free(ret);
				ret = NULL;
			}
		} // if ret
	} // if a && b
	return ret;
}

char* mul_dec_integer_one(char* sbinta, arrtype bintb) {
	BigInt* a = NULL;
	BigInt* b = NULL;
	char* ret = NULL;

	if (sbinta != NULL) {
		a = bigint_initial_dec_string(sbinta);
		if (__bigint_magic_assert(a)) {
			b = bigint_mul_one(a, bintb);
			if (__bigint_magic_assert(b)) {
				ret = bigint_print_return_string(b);
			}
			bigint_cleanup_pointer(&a);
		}
	}

	return  ret;
}

arrtype bigint_length(BigInt* bigint) {
	arrtype length = 0;

	if (__bigint_magic_assert(bigint)) {
		for (length = bigint->arrlen - 1; length >= 0; length--) {
			if (bigint->arr[length] != 0) {
				break;
			}
		}
		length++;
		if (length == 0) {
			length = 1;
		}
	}
	return length;
}

BigInt* bigint_duplicate(BigInt* bigint) {
	BigInt* result = NULL;
	arrtype  i;

	if (__bigint_magic_assert(bigint)) {
		result = (BigInt*)malloc(sizeof(BigInt));

		if (result != NULL) {
			result->arr = (arrtype*)malloc(sizeof(arrtype)*bigint->arrlen);
			if (result->arr == NULL) {
				free(result);
				result = NULL;
			}
			else {
				result->arrlen = bigint->arrlen;
				result->base = bigint->base;
				result->digital = bigint->digital;
				result->sign = bigint->sign;
				for (i = 0; i < bigint->arrlen; i++) {
					result->arr[i] = bigint->arr[i];
				}
			}
		}
	}

	return result;
}

/*
a/b = (sign(a) ^ sign(b))|a|/|b|

|a|/|b| =
(1) [a0 a1 a2 a3 a4 a5 a6 a7 a8 a9] / [b0 b1 b2 b3 b4 b5 b6 b7 b8 b9] =

ai i=0...N-1=0...9, N=10
bj j=0...M-1=0...9, M=10

let tmp = ai

[a0]           [a1]         [a2]           [a3]          [a4]          [a5]          [a6]          [a7]           [a8]         [a9]
-)   [K*b0]       [K*b1]     [K*b2]       [K*b3]      [K*b4]      [K*b5]      [K*b6]      [K*b7]       [K*b8]     [K*b9]
------------------------------------------------------------------------------------------------------------------------------------
[a0-K*b0] [a1-K*b1] [a2-K*b2] [a3-K*b3] [a4-K*b4] [a5-K*b5] [a6-K*b6] [a7-K*b7] [a8-K*b8] [a9-K*b9] => remainder
devider = K

approch mathod of get K is

K ~ (a0 * base + a1) / (b0 * base + b1)

R = a/b ~ gauss(a/10^P)*10^P/gauss(b/10^P)*10^P = gauss(a/10^P)/gauss(b/10^P)

R'=gauss(a/10^P)/gauss(b/10^P) ~ K



(2) [a0 a1 a2 a3 a4 a5 a6 a7 a8 a9] / [b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19] =

reminder is [a0 a1 a2 a3 a4 a5 a6 a7 a8 a9], K=0

(3) [a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 a16 a17 a18 a19] / [b0 b1 b2 b3 b4 b5 b6 b7 b8 b9] =

approch mathod of get K is
Ki [i=0~10(N-M)](count:¢Ü-M+1) => [Ki*bj] [0]*(N-M-i)

Ki = ai*base+a(i+1)/b0*base+b1

Reverse Index function of array(M)
0    ...    i    ... N-1
N-1  ... N-i-1 ...  0
0 1 2 3 4 5
5 4 3 2 1 0
RevInd_M(A) = (A)_M' = M-A-1
RevIndRng_M(A, B) =  (A, B)_M' = (M-B-1, M-A-1)

RevIndRng_M(0, A) =  (0, A)_M' = (M-A-1, M-1)
RevIndRng_N(0, N-M) =  (0, N-M)_N' = (M-1, N-1)

Rev(M-1, N-1) = (N-1, M-1)

0
0 1 2 3 4 5 6 7 8 9 10 11
0 1 2 3 4 5 6 7

0 1
0 1 2 3 4 5 6 7 8 9 10 11
0 1 2 3 4 5 6 7

0 1 2 3 4
0 1 2 3 4 5 6 7 8 9 10 11
0 1 2 3 4 5  6  7

4  3  2 1 0
11 10 9 8 7 6 5 4 3 2 1 0
7 6 5 4 3 2  1 0
(N-1, M-1) = (11, 7)

Ki' = ai * base + a(i-1) / b(M-1) * base + b(M-2)

i' belongs range(0, N-M) count: N-M+1

for i in range(N-1, M-1):
ind = i-N+1
K(ind) = ai * base + a(i-1) / b(M-1) * base + b(M-2)
find = False
#// down test
loop:
result = Ki*b
if result > [ai...a(i+M-1)]:
find = True # ever down
Ki--
else:
break
if find == True:
pass
else:
Ki++
#// up test
loop:
result = Ki*b
if result < [ai...a(i+M-1)]:
Ki++
else:
Ki--
break

K ~ (a0 * base + a1) / (b0 * base + b1)

R = a/b ~ gauss(a/10^N)*10^N/gauss(b/10^N)*10^N = gauss(a/10^N)/gauss(b/10^N)

R'=gauss(a/10^N)/gauss(b/10^N) ~ K

[ai*b0] ... [ai*bj] ... [ai*bM] & [0] * (N-i)


[a0*b0] [a0*b1] [a0*b2]  [a0*b3] [a0*b4] [a0*b5] [a0*b6] [a0*b7] [a0*b8] [a0*b9] [space]   [space]  [space]   [space]   [space]  [space]   [space]   [space] [space]
[a1*b0] [a1*b1]   [a1*b2]  [a1*b3] [a1*b4]  [a1*b5] [a1*b6]  [a1*b7] [a1*b8]  [a1*b9]  [space]  [space]   [space]   [space]  [space]    [space]  [space] [space]  [space]
[a2*b0] []


guass quotient:
a/c ~ (a*10+b)/(c*10+d)
[a(c*10+d)-c(a*10+b)]/c(c*10+d) = ad-cb/c*c*10+cd
(-base**2~base**2)/(1~base**2)
max(ad)
*/
// (R, D) = a/b
// D = gauss(a/b)
// R = {a/b} = reminder(a/b) = a%b
BigInt** bigint_div(BigInt* a, BigInt* b) {
	BigInt* tmp1 = NULL, *tmp2 = NULL;
	BigInt* quotient = NULL;
	BigInt* quotient_tmp = NULL;

	BigInt* reminder = NULL;
	BigInt* reminder_tmp = NULL;

	BigInt** result = NULL;
	arrtype afirst, bfirst;
	arrtype asign, bsign;
	arrtype i;
	arrtype iresult1 = 0, iresult2 = 0;
	arrtype max = 0, min = 0;
	arrtype iguass = 0;

	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {

		result = (BigInt**)malloc(sizeof(BigInt*) * 2);

		if (result) {

			for (i = a->arrlen - 1; i >= 0; i--) {
				if (a->arr[i] != 0) {
					break;
				}
			}
			if (i < 0) {
				afirst = 0;
			}
			else {
				afirst = i;
			}

			for (i = b->arrlen - 1; i >= 0; i--) {
				if (b->arr[i] != 0) {
					break;
				}
			}
			if (i < 0) {
				bfirst = 0;
			}
			else {
				bfirst = i;
			}


			if (afirst < bfirst) {
				// TODO: return 0
				quotient = bigint_initial_dec_string("0");
				reminder = bigint_duplicate(a);
			}
			else {

				if ((bfirst == 0) && (b->arr[bfirst] == 0)) {
					// TODO: return inf
					reminder = NULL;
					quotient = bigint_initial_dec_string("0");
					if (a->sign == 1) {
						quotient->sign = -2;
					}
					else {
						quotient->sign = 2;
					}
				}
				else {
					quotient = bigint_initial_size(afirst - bfirst + 2);
					quotient_tmp = bigint_initial_size(afirst - bfirst + 2);

					asign = a->sign;
					bsign = b->sign;
					a->sign = 0;
					b->sign = 0;

					reminder = bigint_duplicate(a);

					// do div
					for (i = afirst; i >= bfirst; i--) {

						if ((i == afirst) || (reminder->arrlen <= (i + 1))) {
							quotient->arr[i - bfirst] = quotient_tmp->arr[i - bfirst] = reminder->arr[i] / b->arr[bfirst];
						}
						else {
							quotient->arr[i - bfirst] = quotient_tmp->arr[i - bfirst] = (reminder->arr[i + 1] * a->base + reminder->arr[i]) / b->arr[bfirst]; // guass quotient
						}
						tmp1 = bigint_mul(b, quotient_tmp);
						//reminder = bigint_del(a, tmp1);
						//bigint_del_into(reminder, tmp1);
						iresult1 = bigint_compare(tmp1, reminder);

						if (iresult1 > 0) { // b*q > a, continue to find quotient
							max = quotient->arr[i - bfirst] + 1;
							min = 0;
						}
						else if (iresult1 < 0) { // b*q < a, find reminder
												 //quotient->arr[i - bfirst] = 0;
							bigint_del_into(reminder, tmp1);
							quotient_tmp->arr[i - bfirst] = 0;
							bigint_cleanup_pointer(&tmp1);
							continue;
						}
						else { // find result
							bigint_del_into(reminder, tmp1); // solution for bug #1
							bigint_cleanup_pointer(&tmp1);
							quotient_tmp->arr[i - bfirst] = 0;
							break;
						}
						bigint_cleanup_pointer(&tmp1);

						//quotient->arr[i - bfirst] = quotient_tmp->arr[i - bfirst] = (min + max) / ((arrtype)2);

						// binary search
						while ((min < max) && (min != (max - 1))) {
							// multiply test
							//for(j=0;j<)
							quotient->arr[i - bfirst] = quotient_tmp->arr[i - bfirst] = (min + max) / 2;
							tmp1 = bigint_mul(b, quotient_tmp);
							tmp2 = bigint_del(tmp1, reminder);
							tmp2->sign = 0;
							iresult1 = bigint_compare(tmp2, b);
							iresult2 = bigint_compare(tmp1, reminder);

							bigint_cleanup_pointer(&tmp2);

							if (iresult1 < 0) { // b * quotient_tmp < b, approch the result
								if (iresult2 > 0) { // b * quotient_tmp - reminder > reminder, bigger than result, then -1
									min = max = quotient->arr[i - bfirst] - 1;
								}
								else {
									min = max = quotient->arr[i - bfirst];
								}
								break;
							}
							else { // not approch the result, continue to search
								if (iresult2 > 0) {
									max = quotient->arr[i - bfirst];
								}
								else if (iresult2 < 0) {
									min = quotient->arr[i - bfirst];
								}
								else {
									min = max = quotient->arr[i - bfirst];
									min--;
									max++;
								}
							}
							bigint_cleanup_pointer(&tmp1);
						}
						quotient->arr[i - bfirst] = quotient_tmp->arr[i - bfirst] = min;
						tmp1 = bigint_mul(b, quotient_tmp);
						bigint_del_into(reminder, tmp1); //error=> len is changed
						bigint_cleanup_pointer(&tmp1);
						quotient_tmp->arr[i - bfirst] = 0;
					} // for i=(afirst,bfisrt)

					a->sign = asign;
					b->sign = bsign;
					quotient->sign = a->sign ^ b->sign;

					if (a->sign == 1) {
						if (b->sign == 1) {
							bigint_replace(quotient, bigint_add_one(quotient, 1));
						}
						else {
							bigint_replace(quotient, bigint_del_one(quotient, 1));
						}
						reminder_tmp = bigint_duplicate(b);
						reminder_tmp->sign = 0;
						bigint_del_into(reminder_tmp, reminder);
						bigint_replace(reminder, reminder_tmp);
					}

					//tmp1 = bigint_mul(b, quotient);
					//reminder = bigint_del(a, tmp1);
					bigint_cleanup_pointer(&tmp1);

				} // if ((bfirst == 0) && (b->arr[bfirst] == 0)) else
			}// if (afirst < bfirst) else
			result[0] = quotient;
			result[1] = reminder;
		}
	}
	return result;
}

BigInt** bigint_div_one(BigInt* a, arrtype bintb) {
	BigInt* b = NULL;
	BigInt* c = NULL;
	BigInt** ret = NULL;

	b = bigint_initial_dec_string("0");
	c = bigint_add_one(b, bintb);
	ret = bigint_div(a, c);
	bigint_cleanup_pointer(&b);
	bigint_cleanup_pointer(&c);

	return ret;
}

BigInt* bigint_initial_one(arrtype bint) {
	BigInt* tbint = NULL;
	BigInt* rbint = NULL;
	tbint = bigint_initial_dec_string("0");
	rbint = bigint_add_one(tbint, bint);
	bigint_cleanup_pointer(&tbint);
	return rbint;
}

// for result assign to same variable
void bigint_replace(BigInt* a, BigInt* b) {
	if (__bigint_magic_assert(a) && __bigint_magic_assert(b)) {
		bigint_cleanup(a);
		a->arr = b->arr;
		a->arrlen = b->arrlen;
		a->base = b->base;
		a->digital = b->digital;
		a->sign = b->sign;
	}
}

char** div_dec_integer(char* sbinta, char* sbintb) {
	BigInt* a = NULL;
	BigInt* b = NULL;
	BigInt** c = NULL;
	char* quotient = NULL, *reminder = NULL;
	char** result = NULL;

	result = (char**)malloc(sizeof(char*) * 2);

	if (result) {

		a = bigint_initial_dec_string(sbinta);
		if (a) {
			b = bigint_initial_dec_string(sbintb);
			if (b) {
				c = bigint_div(a, b);
				if (c) {
					quotient = bigint_print_return_string(c[0]);
					reminder = bigint_print_return_string(c[1]);
					bigint_cleanup_pointer(&a);
					bigint_cleanup_pointer(&b);
					result[0] = quotient;
					result[1] = reminder;
				}
				else {
					bigint_cleanup_pointer(&a);
					bigint_cleanup_pointer(&b);
					free(result);
					result = NULL;
				}
			}
			else {
				bigint_cleanup_pointer(&a);
				free(result);
				result = NULL;
			}
		}
		else {
			free(result);
			result = NULL;
		}
	}
	return result;
}

char** div_dec_integer_one(char* sbinta, arrtype bintb) {
	BigInt* a = NULL;
	BigInt** c = NULL;
	char* quotient = NULL, *reminder = NULL;
	char** result = NULL;

	result = (char**)malloc(sizeof(char*) * 2);

	if (result) {

		a = bigint_initial_dec_string(sbinta);
		if (a) {
			c = bigint_div_one(a, bintb);
			if (c) {
				quotient = bigint_print_return_string(c[0]);
				reminder = bigint_print_return_string(c[1]);
				bigint_cleanup_pointer(&a);
				result[0] = quotient;
				result[1] = reminder;
			}
			else {
				bigint_cleanup_pointer(&a);
				free(result);
				result = NULL;
			}
		}
		else {
			free(result);
			result = NULL;
		}
	}
	return result;
}

void bigint_add_into(BigInt* a, BigInt* b) {
	bigint_replace(a, bigint_add(a, b));
}

void bigint_add_one_into(BigInt* a, arrtype b) {
	bigint_replace(a, bigint_add_one(a, b));
}

void bigint_del_into(BigInt* a, BigInt* b) {
	bigint_replace(a, bigint_del(a, b));
}

void bigint_del_one_into(BigInt* a, arrtype b) {
	bigint_replace(a, bigint_del_one(a, b));
}

void bigint_mul_into(BigInt* a, BigInt* b) {
	bigint_replace(a, bigint_mul(a, b));
}

void bigint_mul_one_into(BigInt* a, arrtype b) {
	bigint_replace(a, bigint_mul_one(a, b));
}

// just using quotient
void bigint_div_into(BigInt* a, BigInt* b) {
	BigInt** result = bigint_div(a, b);
	if (result) {
		bigint_replace(a, result[0]);
		bigint_cleanup_pointer(result + 1);
		free(result);
	}
}

void bigint_div_one_into(BigInt* a, arrtype b) {
	BigInt** result = bigint_div_one(a, b);
	if (result) {
		bigint_replace(a, result[0]);
		bigint_cleanup_pointer(result + 1);
		free(result);
	}
}

BigInt* bigint_div_return_quotient(BigInt* a, BigInt* b) {
	BigInt** ret = bigint_div(a, b);
	BigInt* quotient = NULL;
	if (ret) {
		quotient = ret[0];
		bigint_cleanup_pointer(&ret[1]);
		free(ret);
	}
	return quotient;
}

BigInt* bigint_div_one_return_quotient(BigInt* a, arrtype bintb) {
	BigInt** ret = bigint_div_one(a, bintb);
	BigInt* quotient = NULL;
	if (ret) {
		quotient = ret[0];
		bigint_cleanup_pointer(&ret[1]);
		free(ret);
	}
	return quotient;
}

BigInt* bigint_div_return_reminder(BigInt* a, BigInt* b) {
	BigInt** ret = bigint_div(a, b);
	BigInt* quotient = NULL;
	if (ret) {
		quotient = ret[1];
		bigint_cleanup_pointer(&ret[0]);
		free(ret);
	}
	return quotient;
}

BigInt* bigint_div_one_return_reminder(BigInt* a, arrtype bintb) {
	BigInt** ret = bigint_div_one(a, bintb);
	BigInt* quotient = NULL;
	if (ret) {
		quotient = ret[1];
		bigint_cleanup_pointer(&ret[0]);
		free(ret);
	}
	return quotient;
}

void bigint_div_into_reminder(BigInt* a, BigInt* b) {
	BigInt** ret = bigint_div(a, b);
	if (ret) {
		bigint_replace(a, ret[1]);
		bigint_cleanup_pointer(&ret[0]);
		free(ret);
	}
}

void bigint_div_one_into_reminder(BigInt* a, arrtype bintb) {
	BigInt** ret = bigint_div_one(a, bintb);
	if (ret) {
		bigint_replace(a, ret[1]);
		bigint_cleanup_pointer(&ret[0]);
		free(ret);
	}
}

BigInt* bigint_div_into_return_reminder(BigInt* a, BigInt* b) {
	BigInt** result = bigint_div(a, b);
	BigInt* ret = NULL;
	if (result) {
		bigint_replace(a, result[0]);
		ret = result[1];
		free(result);
	}
	return ret;
}

BigInt* bigint_div_one_into_return_reminder(BigInt* a, arrtype bintb) {
	BigInt** result = bigint_div_one(a, bintb);
	BigInt* ret = NULL;
	if (result) {
		bigint_replace(a, result[0]);
		ret = result[1];
		free(result);
	}
	return ret;
}

BigInt* bigint_power_one(BigInt* a, arrtype bintb) {
	arrtype i = 0;
	BigInt* bint = NULL;

	if (__bigint_magic_assert(a) && (bintb > 0)) {
		bint = bigint_duplicate(a);
		for (i = 1; i < bintb; i++) {
			bigint_mul_into(bint, a);
		}
	}

	return bint;
}

BigInt* bigint_power(BigInt* a, BigInt* b) {
	BigInt* i = NULL;
	BigInt* bint = NULL;

	if (__bigint_magic_assert(a) && (b->sign == 0)) {
		bint = bigint_duplicate(a);
		for (i = bigint_initial_dec_string("1"); bigint_compare(i, b) < 0; bigint_add_one_into(i, 1)) {
			bigint_mul_into(bint, a);
		}
		bigint_cleanup_pointer(&i);
	}

	return bint;
}

void bigint_power_one_into(BigInt* a, arrtype bintb) {
	arrtype i = 0;
	BigInt* bint = NULL;

	if (__bigint_magic_assert(a) && (bintb > 0)) {
		bint = bigint_duplicate(a);
		//printf("5^%d = ", i + 1); bigint_print(bint); printf("\n");
		for (i = 1; i < bintb; i++) {
			bigint_mul_into(bint, a);
			//printf("5^%d = ", i+1); bigint_print(bint); printf("\n");
		}
		bigint_replace(a, bint);
	}
}

void bigint_power_into(BigInt* a, BigInt* b) {
	BigInt* i = NULL;
	BigInt* bint = NULL;

	if (__bigint_magic_assert(a) && (b->sign == 0)) {
		bint = bigint_duplicate(a);
		for (i = bigint_initial_dec_string("1"); bigint_compare(i, b) < 0; bigint_add_one_into(i, 1)) {
			bigint_mul_into(bint, a);
		}
		bigint_cleanup_pointer(&i);
		bigint_replace(a, bint);
	}
}

char* pwd_dec_integer(char* sbinta, char* sbintb) {
	BigInt* a = NULL;
	BigInt* b = NULL;
	char* ret = NULL;

	if (sbinta && sbintb) {
		a = bigint_initial_dec_string(sbinta);
		b = bigint_initial_dec_string(sbintb);

		bigint_power_into(a, b);

		ret = bigint_print_return_string(a);

		bigint_cleanup_pointer(&a);
		bigint_cleanup_pointer(&b);
	}

	return ret;
}

char* pwd_dec_integer_one(char* sbinta, arrtype bintb) {
	BigInt* a = NULL;
	char* ret = NULL;

	if (sbinta && bintb) {
		a = bigint_initial_dec_string(sbinta);

		bigint_power_one_into(a, bintb);

		ret = bigint_print_return_string(a);

		bigint_cleanup_pointer(&a);
	}

	return ret;
}

BigInt* bigint_initial_ten_base_one(arrtype bintb, arrtype basen) {
	BigInt* bigint = NULL;
	arrtype* tmp = NULL;
	arrtype space_digital = 0;
	arrtype space_detail = 0;
	arrtype base = 1, itmp,i;

	if (basen > 0) {
		bigint = bigint_initial_one(bintb);
		if (bigint) {
			space_digital = basen / bigint->digital;
			space_detail = basen % bigint->digital;
			if (space_detail > 0) {
				for (itmp = space_detail; itmp; itmp--) {
					base *= 10;
				}
				bigint_mul_one_into(bigint, base);

				tmp = (arrtype*)malloc(sizeof(arrtype)*(bigint->arrlen + space_digital));
				if (tmp) {
					for (i = 0; i < space_digital; i++) {
						tmp[i] = 0;
					}

					for (i = space_digital; i < (space_digital + bigint->arrlen); i++) {
						tmp[i] = bigint->arr[i - space_digital];
					}
					free(bigint->arr);
					bigint->arr = tmp;
					bigint->arrlen += space_digital;
				}
				else {
					bigint_cleanup_pointer(&bigint);
				}
			}
		}
	}
	else if (basen == 0) {
		bigint = bigint_initial_one(bintb);
	}
	else {
		bigint = bigint_initial_zero();
	}

	return bigint;
}

arrtype bigint_base10_length(BigInt* bigint) {
	arrtype head = 0;
	arrtype tmp = 0;
	arrtype length = 0;

	if (__bigint_magic_assert(bigint)) {
		for (head = bigint->arrlen - 1; head >= 0; head--) {
			if (bigint->arr[head] != 0) {
				break;
			}
		}
		if (head < 0) {
			head = 0;
		}
		length = 0;
		tmp = bigint->arr[head];
		while (tmp) {
			length++;
			tmp /= 10;
		}
		length += head* bigint->digital;
	}
	return length;
}

void bigint_write_digital(BigInt* bigint, arrtype index, arrtype value) {
	arrtype value_item = 0;
	arrtype value_sindex = 0;
	arrtype base = 1;
	arrtype i, tmp;
	if (__bigint_magic_assert(bigint)) {
		value_item = (index / bigint->digital);
		value_sindex = (index % bigint->digital);
		if ((value >= 0) && (value <= 9)) {
			for (i = 0; i < value_sindex; i++) {
				base *= 10;
			}
			tmp = bigint->arr[value_item];
			if (value_sindex == 0) {
				bigint->arr[value_item] = (tmp / 10) * 10 + value;
			}
			else {
				bigint->arr[value_item] = (tmp / (base * 10))*(base * 10) + value*base + tmp % base;
			}
		}
	}
}

arrtype bigint_read_digital(BigInt* bigint, arrtype index) {
	arrtype value_item = 0;
	arrtype value_sindex = 0;
	arrtype base = 1;
	arrtype i;
	arrtype ret = 0;

	if (__bigint_magic_assert(bigint)) {
		value_item = (index / bigint->digital);
		value_sindex = index % bigint->digital;
		for (i = 0; i < value_sindex; i++) {
			base *= 10;
		}
		ret = ((bigint->arr[value_item]) / base) % 10;
	}

	return ret;
}

BigInt* bigint_sqrt(BigInt* bigint) {
	arrtype head = bigint_length(bigint) - 1;
	arrtype digital_back = 0;
	BigInt* result = NULL, *tmp = NULL;

	// get first two number 
	arrtype headvalue = 0;

	if (__bigint_magic_assert(bigint)) {
		if (head > 1) {
			headvalue = bigint->arr[head] * bigint->base + bigint->arr[head - 1];
			digital_back = (head - 1)*bigint->digital;
			if (((head - 1)*bigint->digital) % 2 == 1) { // odd value -> root will odd, not direct method for big sqrt
				digital_back++;
				headvalue /= 10;
			}

			// get first value of sqrt
			result = bigint_initial_ten_base_one(headvalue / 2, digital_back / 2);

			//printf("sqrt(%s) = ", number); bigint_print(tmp); printf("\n"); system("pause");

			while (1) {
				tmp = bigint_duplicate(result);
				bigint_mul_into(tmp, result); // value^2
				bigint_add_into(tmp, bigint);
				bigint_div_into(tmp, result);
				bigint_div_one_into(tmp, 2);

				if (bigint_compare(result, tmp) == 0) {
					bigint_cleanup_pointer(&tmp);
					break;
				}

				bigint_replace(result, tmp);
				tmp = NULL;
				//printf("sqrt(%s) = ", number); bigint_print(tmp); printf("\n"); system("pause");
			}

		}
		else if (head == 1) {
			headvalue = bigint->arr[head] * bigint->base + bigint->arr[head - 1];
			result = bigint_duplicate(bigint);
			if (result) {
				result->arr[head] = 0;
				result->arr[head - 1] = (int)sqrt(headvalue);
			}
		}
		else {
			result = bigint_duplicate(bigint);
			if (result) {
				result->arr[head] = (int)sqrt(bigint->arr[head]);
			}
		}
	}
	return result;
}

void bigint_sqrt_into(BigInt* bigint) {
	arrtype head = bigint_length(bigint) - 1;
	arrtype digital_back = 0;
	BigInt* tmp = NULL, *tmp2 = NULL;

	// get first two number 
	arrtype headvalue = 0;

	if (__bigint_magic_assert(bigint)) {
		if (head > 1) {
			headvalue = bigint->arr[head] * bigint->base + bigint->arr[head - 1];
			digital_back = (head - 1)*bigint->digital;
			if (((head - 1)*bigint->digital) % 2 == 1) { // odd value -> root will odd, not direct method for big sqrt
				digital_back++;
				headvalue /= 10;
			}

			// get first value of sqrt
			tmp = bigint_initial_ten_base_one(headvalue / 2, digital_back / 2);

			//printf("sqrt(%s) = ", number); bigint_print(tmp); printf("\n"); system("pause");

			while (1) {
				tmp2 = bigint_duplicate(tmp);
				bigint_mul_into(tmp2, tmp); // value^2
				bigint_add_into(tmp2, bigint);
				bigint_div_into(tmp2, tmp);
				bigint_div_one_into(tmp2, 2);

				if (bigint_compare(tmp, tmp2) == 0) {
					bigint_cleanup_pointer(&tmp2);
					break;
				}

				bigint_replace(tmp, tmp2);
				tmp2 = NULL;
				//printf("sqrt(%s) = ", number); bigint_print(tmp); printf("\n"); system("pause");
			}
			bigint_replace(bigint, tmp);
		}
		else if (head == 1) {
			headvalue = bigint->arr[head] * bigint->base + bigint->arr[head - 1];
			bigint->arr[head] = 0;
			bigint->arr[head - 1] = (int)sqrt(headvalue);
		}
		else {
			bigint->arr[head] = (int)sqrt(bigint->arr[head]);
		}
	}
}

char* sqrt_dec_integer(char* decstr) {
	BigInt* bigint = bigint_initial_dec_string(decstr);
	arrtype head = bigint_length(bigint) - 1;
	arrtype digital_back = 0;
	BigInt* tmp = NULL, *tmp2 = NULL;
	char* result = NULL;

	// get first two number 
	arrtype headvalue = 0;
	if (head > 1) {
		headvalue = bigint->arr[head] * bigint->base + bigint->arr[head - 1];
		digital_back = (head - 1)*bigint->digital;
		if (((head - 1)*bigint->digital) % 2 == 1) { // odd value -> root will odd, not direct method for big sqrt
			digital_back++;
			headvalue /= 10;
		}

		// get first value of sqrt
		tmp = bigint_initial_ten_base_one(headvalue / 2, digital_back / 2);

		//printf("sqrt(%s) = ", number); bigint_print(tmp); printf("\n"); system("pause");

		while (1) {
			tmp2 = bigint_duplicate(tmp);
			bigint_mul_into(tmp2, tmp); // value^2
			bigint_add_into(tmp2, bigint);
			bigint_div_into(tmp2, tmp);
			bigint_div_one_into(tmp2, 2);

			if (bigint_compare(tmp, tmp2) == 0) {
				bigint_cleanup_pointer(&tmp2);
				break;
			}

			bigint_replace(tmp, tmp2);
			tmp2 = NULL;
			//printf("sqrt(%s) = ", number); bigint_print(tmp); printf("\n"); system("pause");
		}
		bigint_replace(bigint, tmp);

	}
	else if (head == 1) {
		headvalue = bigint->arr[head] * bigint->base + bigint->arr[head - 1];
		bigint->arr[head] = 0;
		bigint->arr[head - 1] = (int)sqrt(headvalue);
	}
	else {
		bigint->arr[head] = (int)sqrt(bigint->arr[head]);
	}

	// do somthing when done
	result = bigint_print_return_string(bigint);

	bigint_cleanup_pointer(&bigint);

	return result;
}

BigInt* bigint_initial_hex_string(char* hexstr) {
	arrtype i = 0, j = 0;
	BigInt* ret = NULL;
	int c;
	char shex[17] = "0123456789ABCDEF";

	arrtype convert[256] = { 0 };

	if (hexstr) {
		for (i = 0; i < 16; i++) {
			convert[shex[i]] = i;
		}
		ret = bigint_initial_zero();
		if (ret) {
			for (i = 0; hexstr[i]; i++) {
				c = hexstr[i];
				if ((c >= 'a') && (c <= 'f')) {
					c = c - 'a' + 'A';
				}
				bigint_mul_one_into(ret, 16);
				bigint_add_one_into(ret, convert[c]);
			}
		}
	}

	return ret;
}

// return n!
BigInt* bigint_factor(arrtype n) {
	BigInt* bint = 0;
	arrtype i = 0;

	bint = bigint_initial_one(1);

	if (bint) {
		for (i = 2; i <= n; i++) {
			bigint_mul_one_into(bint, i);
		}
	}

	return bint;
}

// return n!
char* factor_return_dec_string(arrtype n) {
	BigInt* bint = 0;
	char* decstrret = NULL;
	arrtype i;

	bint = bigint_initial_one(1);

	if (bint) {
		for (i = 2; i <= n; i++) {
			bigint_mul_one_into(bint, i);
		}
		decstrret = bigint_print_return_string(bint);
		bigint_cleanup_pointer(&bint);
	}

	return decstrret;
}

BigInt* bigint_combination(arrtype n, arrtype m) {
	BigInt* bint = NULL;
	BigInt* rp = NULL;
	BigInt* ndrp = NULL;
	BigInt** divs = NULL;

	bint = bigint_factor(n);
	rp = bigint_factor(m);
	ndrp = bigint_factor(n - m);
	bigint_mul_into(rp, ndrp);

	divs = bigint_div(bint, rp);

	bigint_replace(bint, divs[0]);
	bigint_cleanup_pointer(&divs[1]);
	free(divs);
	divs = NULL;

	bigint_cleanup_pointer(&rp);
	bigint_cleanup_pointer(&ndrp);

	return bint;
}
