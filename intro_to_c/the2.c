#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 210
#define AND '&'
#define OR '|'
#define IMP '>'
#define NEG '-'
#define LEFTP '('
#define RIGHTP ')'
#define COMMA ','
#define NULLT '\0'

/*parser, hand made*/
void parser(char *string, char *token, char delimeter, int *index, char *remaining){
	while((*(string + *index) != delimeter) && (*(string + *index) != NULLT)){
		*token = *(string + *index);
		(*index)++;
		token++;
	}
	*token = NULLT;
	if(*(string + *index) != NULLT){
		(*index)++;
	}
	while((*(string + *index) != NULLT)){
		*remaining = *(string + *index);
		remaining++;
		string++;
	}
	*remaining = NULLT;
}

/* check if it is a letter, aka atomic*/
int is_atomic(char *expression){
	if(('A' <= *expression && *expression <= 'Z')){
		return 1;
	}
	return 0;
}

/* determine the top level connective*/
char* determine_connective(char *expression){
	int depth = 0;
	char *connective;
	if (*expression == NEG){
		return expression;
	}
	while(*expression){
		if(*expression == LEFTP){
			depth++;
		}else if(*expression == RIGHTP){
			depth--;
		}else if((depth == 1) && (*expression == OR || *expression == AND || *expression == IMP)){
			connective = expression;
		}
		expression++;
	}
	return connective;
}
/* not on both*/
void not(char *expression){
	while(*expression){
		*expression = *(expression + 1);
		expression++;
	}
}
/*and on left or on right*/
void and(char *expression, char *connective){
	*connective = COMMA;
	while(*expression){
		*expression = *(expression+1);
		expression++;
	}
	*(expression-2) = NULLT;
}
/* implication on both*/
void imp(char *expression, char *connective){
	int t;
	*connective = OR;
	if(*(expression + 1) == NEG){
		while(*(expression + 1)){
			*(expression + 1) = *(expression + 2);
			expression++;
		}
		*(expression) = NULLT;
	}else{
		t = strlen(expression);
		*(expression + t + 1) = NULLT;
		while(t){
			*(expression + t) = *(expression + t -1);
			t--;
		}
		*(expression + 1) = NEG;
	}
}

/*or on left and on right*/
void or(char *expression, char *connective, char *op1, char *op2){
	expression++;
	while(expression != connective){
		*(op1++) = *(expression++);
	}
	*(op1) = NULLT;
	while(*expression){
		*(op2++) = *(++expression);
	}
	*(op2 - 2) = NULLT;
}




/* check the return true case*/
int is_proved_valid(char *lhs, char *rhs){
	int i = 0, j = 0;
	char *lefttoken, *righttoken, *remaining;
	lefttoken = malloc(MAX_LENGTH);
	righttoken = malloc(MAX_LENGTH);
	remaining = malloc(MAX_LENGTH);
	parser(lhs, lefttoken, COMMA, &i, remaining);
	while (*lefttoken != NULLT){
		parser(rhs, righttoken, COMMA, &j, remaining);
		while (*righttoken != NULLT){
			if (!strcmp(lefttoken, righttoken)){
				free(lefttoken);free(righttoken);free(remaining);
				return 1;
			}
			parser(rhs, righttoken, COMMA, &j, remaining);
		}
		j = 0;
		parser(lhs, lefttoken, COMMA, &i, remaining);
	}
	free(lefttoken);free(righttoken);free(remaining);
	return 0;
}

/*check the return false case*/
int is_proved_invalid(char *lhs, char *rhs){
	int i = 0, j = 0, m = 0, n = 0;
	char *lefttoken, *righttoken, *remaining;
	lefttoken = malloc(MAX_LENGTH);
	righttoken = malloc(MAX_LENGTH);
	remaining = malloc(MAX_LENGTH);
	parser(lhs, lefttoken, COMMA, &i, remaining);
	while (*lefttoken != NULLT){
		if(!is_atomic(lefttoken)){
			free(lefttoken);free(righttoken);free(remaining);
			return 0;
		}
		parser(lhs, lefttoken, COMMA, &i, remaining);
	}
	parser(rhs, righttoken, COMMA, &j, remaining);
	while (*righttoken != NULLT){
		if(!is_atomic(righttoken)){
			free(lefttoken);free(righttoken);free(remaining);
			return 0;
		}
		parser(rhs, righttoken, COMMA, &j, remaining);
	}
	parser(lhs, lefttoken, COMMA, &m, remaining);
	while(*lefttoken != NULLT){
		parser(rhs, righttoken, COMMA, &n, remaining);
		while(*righttoken != NULLT){
			if(!strcmp(lefttoken,righttoken)){
				free(lefttoken);free(righttoken);free(remaining);
				return 0;
			}
			parser(rhs, righttoken, COMMA, &n, remaining);
		}
		n = 0;
		parser(lhs, lefttoken, COMMA, &m, remaining);
	}
	free(lefttoken);free(righttoken);free(remaining);
	return 1;
}

int wang(char *lhs, char *rhs){
	/*declarations*/
	char token[MAX_LENGTH], remaining[MAX_LENGTH], op1[MAX_LENGTH], op2[MAX_LENGTH], sub1[MAX_LENGTH], sub2[MAX_LENGTH], newlhs[MAX_LENGTH] = {0}, newrhs[MAX_LENGTH] = {0}, extra[MAX_LENGTH];
	char *connective;
	int i = 0, j = 0;
	/*memory*/
	/*to see what is happening*/
	/*base case*/
	if(is_proved_valid(lhs, rhs)){
		return 1;
	/*another base case*/
	}else if(is_proved_invalid(lhs, rhs)){
		return 0;
	}else{
		/*do the necessary transformations on the left hand side*/
		parser(lhs, token, COMMA, &i, remaining);
		while(*token != NULLT){
			if(is_atomic(token)){
				strcat(newlhs, token);strcat(newlhs, ",");
			}else{
				connective = determine_connective(token);
				if(*connective == NEG){
					not(token);
					strcat(rhs, token);strcat(rhs, ",");
				}else if(*connective == AND){
					and(token, connective);
					strcat(newlhs, token);strcat(newlhs, ",");
				}else if(*connective == IMP){
					imp(token, connective);
					strcat(newlhs, token);strcat(newlhs, ",");
				}else if(*connective == OR){
					/* I think this is problematic*/
					or(token, connective, op1, op2);
					strcpy(sub1, newlhs); strcat(sub1, op1);strcat(sub1, ",");strcat(sub1, remaining);
					strcpy(sub2, newlhs); strcat(sub2, op2);strcat(sub2, ",");strcat(sub2, remaining);
					strcpy(extra, rhs);
					return (wang(sub1, rhs)) && (wang(sub2, extra));
				}
			}
			parser(lhs, token, COMMA, &i, remaining);
		}
		/*do the necessary transformations on the right hand side*/
		parser(rhs, token, COMMA, &j, remaining);
		while(*token != NULLT){
			if(is_atomic(token)){
				strcat(newrhs, token);strcat(newrhs, ",");
			}else{
				connective = determine_connective(token);
				if(*connective == NEG){
					not(token);
					strcat(newlhs, token);strcat(newlhs, ",");
				}else if(*connective == OR){
					and(token, connective);
					strcat(newrhs, token);strcat(newrhs, ",");
				}else if(*connective == IMP){
					imp(token, connective);
					strcat(newrhs, token);strcat(newrhs, ",");
				}else if(*connective == AND){
					/* and also this may be problematic*/
					or(token, connective, op1, op2);
					strcpy(sub1, newrhs);strcat(sub1, op1);strcat(sub1, ",");strcat(sub1, remaining);
					strcpy(sub2, newrhs);strcat(sub2, op2);strcat(sub2, ",");strcat(sub2, remaining);
					strcpy(extra ,newlhs);
					return wang(newlhs, sub1) && wang(extra, sub2);
				}
			}
			parser(rhs, token, COMMA, &j, remaining);
		}
	}
	return wang(newlhs, newrhs);
}
int main(){
	/* declarations*/
	char lhs[MAX_LENGTH], rhs[MAX_LENGTH];
	int ch, i = 0, j = 0, t;
	/* memory*/
	/* get the input*/
	while((ch = getchar()) != '#'){
		*(lhs + i) = ch;
		i++;
	}
	*(lhs + i) = NULLT;
	while((ch = getchar()) != '\n' && ch != EOF){
		*(rhs + j) = ch;
		j++;
	}
	*(rhs + j) = NULLT;
	strcat(lhs, ",");
	strcat(rhs, ",");
	/* DO SOME SHIT*/
	if((t = wang(lhs, rhs))){
		printf("T");
	}else{
		printf("F");
	}
	return 0;
}
