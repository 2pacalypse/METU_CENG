/*
 * algohw.cpp
 *
 *  Created on: 18 Ara 2016
 *      Author: samsung-pc
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <stack>


using namespace std;

int *parent, *rank;

int *FSM1StateTransition;
int *FSM2StateTransition;

char *fsm1states;
char *fsm2states;

char *x;



/* input handling */

int readNumber(ifstream& input){
	int ch;
	int sum = 0;
	ch = input.get();
	while('0' <= ch && ch <= '9'){
		sum = 10*sum + ch - '0';
		ch = input.get();
	}
	return sum;
}

void readLine(ifstream& input, char* array){
	int ch;
	int sum = 0;
	ch = input.get();
	while(('0' <= ch && ch <= '9') || ch == ' '){
		sum =  10*sum + ch - '0';
		if(ch == ' '){
			sum = (sum + '0' - ch)/10;
			array[sum] = 1;
			sum = 0;
		}
		ch = input.get();
	}
	array[sum] = 1;
}

/* disjoint set functions */
void makeset(int x){
	parent[x] = x;
	rank[x] = 0;
}

void link(int x, int y){
	if(rank[x] > rank[y]){
		parent[y] = x;
	}else{
		parent[x] = y;
	}
	if(rank[x] == rank[y])
		rank[y]++;
}

int findset(int x){
	if(parent[x] != x)
		parent[x] = findset(parent[x]);
	return parent[x];
}

void setunion(int x, int y){
	link(findset(x), findset(y));
}



int main(int argc, char*argv[]){
	clock_t tStart = clock();
	/* open files */

	ifstream f1, f2;
	f1.open(argv[1], ios::in);
	f2.open(argv[2], ios::in);


	/* read first three line, state number, symbol number, start symbol */

	int numberOfFSM1States = readNumber(f1);
	int numberOfFSM1Symbols = readNumber(f1);
	int FSM1StartState = readNumber(f1);

	int numberOfFSM2States = readNumber(f2);
	int numberOfFSM2Symbols = readNumber(f2);
	int FSM2StartState = readNumber(f2);


	/* memory allocation */

	parent = new int[numberOfFSM1States + numberOfFSM2States];
	rank = new int[numberOfFSM1States + numberOfFSM2States];
	FSM1StateTransition = new int[numberOfFSM1States*numberOfFSM1Symbols];
	FSM2StateTransition = new int[numberOfFSM2States*numberOfFSM2Symbols];
	fsm1states = new char[numberOfFSM1States];
	fsm2states = new char[numberOfFSM2States];
	x = new char[numberOfFSM1States + numberOfFSM2States];
	memset(x, 0, numberOfFSM1States + numberOfFSM2States);
	memset(fsm1states, 0, numberOfFSM1States);
	memset(fsm2states, 0, numberOfFSM2States);

	/* read final states */

	readLine(f1, fsm1states);
	readLine(f2, fsm2states);


	/* read state transitions */


	for(int i = 0; i < numberOfFSM1States; i++){
		for(int j = 0; j < numberOfFSM1Symbols; j++){
			*(FSM1StateTransition + i*numberOfFSM1Symbols+j) = readNumber(f1);
		}
	}


	for(int i = 0; i < numberOfFSM2States; i++){
		for(int j = 0; j < numberOfFSM2Symbols; j++){
			*(FSM2StateTransition + i*numberOfFSM2Symbols+j) = readNumber(f2);
		}
	}

	/* algorithm begins */

		stack<int> s;
		for(int i = 0; i < numberOfFSM1States + numberOfFSM2States; i++)
			makeset(i);

		s.push(FSM1StartState);
		s.push(FSM2StartState);

		setunion(FSM1StartState, numberOfFSM1States + FSM2StartState);

		while(!s.empty()){

			int q2 = s.top();
			s.pop();
			int q1 = s.top();
			s.pop();



			for(int i = 0; i < numberOfFSM1Symbols;i++){
				int r1 = findset(*(FSM1StateTransition+q1*numberOfFSM1Symbols+i));
				int r2 = findset(*(FSM2StateTransition+q2*numberOfFSM2Symbols+i) + numberOfFSM1States);

				if(r1 != r2){
					setunion(r1, r2);
					s.push(*(FSM1StateTransition+q1*numberOfFSM1Symbols+i));
					s.push(*(FSM2StateTransition+q2*numberOfFSM2Symbols+i));
				}

			}
		}


		/* check sets */


		for(int i = 0; i < numberOfFSM1States;i++){
			if(x[parent[i]] == 0){
				x[parent[i]] = fsm1states[i] ?  1: 2; // if final mark as 1, if not final mark as 2
			}
			else if((x[parent[i]] ==  1 && fsm1states[i] == 0) || (x[parent[i]] ==  2 && fsm1states[i] == 1)){
				cout<< "notequal"<<endl;
				return 0;
			}
		}

		for(int i = 0; i < numberOfFSM2States;i++){
				if(x[parent[i]] == 0){
					x[parent[i]] = fsm2states[i] ?  1: 2; // if final mark as 1, if not final mark as 2
				}
				else if((x[parent[numberOfFSM1States+i]] ==  1 && fsm2states[i] == 0) || (x[parent[numberOfFSM1States+i]] ==  2 && fsm2states[i] == 1)){
				cout<< "notequal"<<endl;
					return 0;
				}
			}



		cout<< "equal"<<endl;
		delete[] parent; delete[] rank; delete[] FSM1StateTransition; delete[] FSM2StateTransition; delete[] fsm1states; delete[] fsm2states; delete[] x;

	return 0;
}
