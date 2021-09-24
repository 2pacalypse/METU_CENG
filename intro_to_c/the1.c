#include <stdio.h>
#define SIZE 50
#define BASE 35

/* check if an array is full of zeros*/
int isZero(char array[], int length){
	int i = 0;
	while(i < length){
		if (array[i++] != 0){
			return 0;
		}
	}
	return 1;
}
/* shift an array to get rid of the leading zeros, also change the upper bound */
void shiftArray(char array[], int* length){
	int i = 0, k = 0;
	while (i < *length){
		if (array[i] == 0){
			k++;
		}else{
			break;
		}
		i++;
	}
	for(i = 0; i < *length; i++){
		array[i] = array[k+i];
	}
	*length = i - k;
}

/* return remainder, quotient is the side effect*/
int division(char dividend[],char quo[], int divisor, int* length){
	int i = 0, j = 0, temp = 0, remainder;
	while (i < *length){
		temp = 10 * temp + dividend[i];
		if (temp >= divisor){
			quo[j++] = temp / divisor;
			temp = temp % divisor;
		}else{
			quo[j++] = 0;
		}
		i++;
	}
	remainder = temp;
	return remainder;
}

int main(){
	/* declarations*/
	char decimal[SIZE], quotient[SIZE], remainders[SIZE];
	int ch, i = 0, j, k = 0;
	/* input the number */
	while ((ch = getchar()) != '\n' && ch != EOF){
		if ('0' <= ch && ch <= '9'){
			decimal[i++] = ch - '0';
		}
	}
	/* magic*/
	shiftArray(decimal, &i);
	remainders[k++] = division(decimal, quotient, 2, &i);
	for (j = 3; j < BASE + 2; j++){
		if(!isZero(quotient, i)){
			shiftArray(quotient, &i);
			remainders[k++] = division(quotient, quotient, j, &i);
		}
	}
	/* print it out*/
	for (j = 0; j < k; j++){
		if (0 <= remainders[k-j-1] && remainders[k-j-1] <= 9){
			printf("%d", remainders[k-j-1]);
		}else{
			printf("%c", remainders[k-j-1] + 55);
		}
	}
	return 0;
}
