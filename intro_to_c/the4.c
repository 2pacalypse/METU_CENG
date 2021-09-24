#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define COMPOSITE 1
#define BASIC 2
#define COMPLEX 3
#define CHILD 4
#define SIBLING 5
#define MAX 25
#define n(x) n->x
#define information(x) information->x
#define sibling(x) sibling->x
#define firstkid(x) firstkid->x
#define pn(x) (*n)->x
#define temp(x) temp->x



/* STRUCTURE DEFINITIONS*/
typedef struct data{
	int dtype;
	double price;
	int quantity;
	char *name;
} data;

typedef struct node{
	struct node *firstkid;
	struct node *sibling;
	struct node *firstkidback;
	struct node *siblingback;
	struct data *information;
}node;

/* GLOBAL DECLARATIONS*/
node *root = NULL;
node *current = NULL;

/* NEEDED FUNCTION PROTOTYPES*/
int identify_unknown(node *);


/* NODE FUNCTIONS*/
node* makenode(){
	node *n;
	n = malloc(sizeof(node));
	n(firstkid) = NULL;
	n(firstkidback) = NULL;
	n(sibling) = NULL;
	n(siblingback) = NULL;
	n(information) = malloc(sizeof(data));
	n(information(name)) = NULL;
	n(information(price)) = 0;
	n(information(quantity)) = 0;
	return n;
}

void add_sibling(node *n){
	while(n(sibling) != NULL){
		n = n(sibling);
	}
	n(sibling) = makenode();
	n(sibling(siblingback)) = n;
	current = n(sibling);
}

void add_firstkid(node **tree, node *n){
	if(*tree == NULL){
		*tree = makenode();
		current = *tree;
	}else{
		n(firstkid) = makenode();
		n(firstkid(firstkidback)) = n;
		current = n(firstkid);
	}
}

void insert_c(node *n, char *name, int q){
	n(information(name)) = name;
	n(information(quantity)) = q;
	n(information(price)) = 0;
	n(information(dtype)) = COMPOSITE;
}

void insert_b(node *n, char *name, int q, double p){
	n(information(name)) = name;
	n(information(quantity)) = q;
	n(information(price)) = p;
	n(information(dtype)) = BASIC;
}

void go_up(node **n){
	while(pn(siblingback)){
		(*n) = pn(siblingback);
	}
	(*n) = pn(firstkidback);
}

node* search(node *n, char *name){
	node *temp;
	if(n == NULL){
		return NULL;
	}else if(!(strcmp(n(information(name)), name)) && !(identify_unknown(n))){
		return n;
	}else{
		temp = search(n(firstkid), name);
		return (temp) ? (temp) : (search(n(sibling), name));
	}

}

void destroy_tree(node *n){
	if(n == NULL){
		return;
	}else{
		destroy_tree(n(firstkid));
		destroy_tree(n(sibling));
		free(n(information(name)));
		free(n(information));
		free(n);
	}
}

/* IDENTIFY FUNCTIONS*/
int identify_part_type(char *token){
	while(*token){
		if(*token == '['){
			return BASIC;
		}
		token++;
	}
	return COMPOSITE;
}

int identify_node_type(char *token){
	int i = 1;
	while(*(token + i)){
		if(*(token + i) == ','){
			return COMPLEX;
		}
		i++;
	}
	if(*token == ','){
		return SIBLING;
	}
	return CHILD;


}

int identify_quantity(char *token){
	int type = identify_node_type(token), sum = 0;
	if(type == CHILD){
		if(isupper(*token)){
			return 1;
		}else{
			while(*token != '*'){
			sum = 10 * sum + (*token) - '0';
			token++;
			}
		return sum;
		}
	}else{
		token++;
		if(isupper(*token)){
			return 1;
		}else{
			while(*token != '*'){
				sum = 10 * sum + (*token) - '0';
				token++;
			}
			return sum;
		}
	}
}

double identify_basic_price(char *token){
	double sum;
	char *p;
	while(*token != '['){
		token++;
	}
	token++;
	return sum = strtod(token, &p);
}

char* identify_name(char *token){
	int i = 0;
	char *temp = malloc(sizeof(char)*MAX);
	while(!isupper(*token)){
		token++;
	}
	while(isupper(*token)){
		*(temp + i) = *token;
		i++, token++;
	}
	temp[i] = '\0';
	return temp;
}

int identify_unknown(node *n){
	if(n(firstkid) == NULL && n(information(dtype)) == COMPOSITE){
		return 1;
	}
	return 0;
}


/*TOKENIZING AND PARSING THE INPUT*/
void cparse(char *arr){
	int i = 0, parttype;
	char temp[2*MAX] = {0};
	if(*arr == ','){
		arr++;
		while(*arr != ',' && *arr != '\0'){
			*(temp + i) = *arr;
			arr++, i++;
		}
		parttype = identify_part_type(temp);
		add_sibling(current);
		if(parttype == COMPOSITE){
			insert_c(current, identify_name(temp), identify_quantity(temp));
		}else{
			insert_b(current, identify_name(temp), identify_quantity(temp), identify_basic_price(temp));
		}
		cparse(arr);
		return ;
	}else if(isupper(*arr) || isdigit(*arr)){
		while(*arr != ',' && *arr != '\0'){
			*(temp + i) = *arr;
			arr++, i++;
		}
		parttype = identify_part_type(temp);
		add_firstkid(&root, current);
		if(parttype == COMPOSITE){
			insert_c(current, identify_name(temp), identify_quantity(temp));
		}else{
			insert_b(current, identify_name(temp), identify_quantity(temp), identify_basic_price(temp));
		}
		cparse(arr);
		return;
	}
}

void parse(){
	int ch, i = 0, mem = 64;
	char *array = malloc(mem);
	while ((ch = getchar()) != EOF){
	if(isspace(ch)){
		continue;
	}else if(ch == ')'){
		*(array + i) = '\0';
		cparse(array);go_up(&current);free(array);parse();return;
	}else if(ch != '(' && ch != ']'){
		array[i++] = ch;
		if(mem == i){
			mem *= 2;
			array = realloc(array, mem);
		}
	}else{
		*(array + i) = '\0';
		cparse(array);free(array);parse();return;
		}
	}
	free(array);
}


/* CALCULATION FUNCTIONS*/
double calculate_basic_price(node *n){
	return n(information(quantity)) * n(information(price));
}

double calculate_price(node *n){
	node *temp;
	if(n == NULL){
		return 0;
	}else if(n(information(dtype)) == BASIC){
		return  calculate_basic_price(n) + calculate_price(n(sibling));
	}else if(!identify_unknown(n)){
		return n(information(quantity)) * calculate_price(n(firstkid)) + calculate_price(n(sibling)) ;
	}else{
		temp = search(root, n(information(name)));
		if(temp(information(dtype)) == COMPOSITE){
			return n(information(quantity)) * calculate_price(temp(firstkid)) +  calculate_price(n(sibling));
		}else{
			return (n(information(quantity)))*(calculate_basic_price(temp))/(temp(information(quantity))) + calculate_price(n(sibling));
		}
	}
}



int main(){
	parse();
	printf("%lf", calculate_price(root));
	destroy_tree(root);
	return 0;
}
