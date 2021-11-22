#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_parser.h"
#include "account.h"
#include <pthread.h>

#include <unistd.h>

#define MAX_THREAD 10

typedef struct{
	int verif;
	int index;
}verIn;

/* Part 1 */
void printAccs(int numAccs, account *accList);
void withdraw(int index, account *accList, double amt);
void deposit(int index, account *accList, double amt);
void transfer(int index, account *accList, double amt, char *acc2, int numAccs);
void addRewardToBalance(account *accList, int numAccs);
void printBalances(account *accList, int numAccs);
verIn verify(int numAccs, account *accList, char *acc, char *pw);

/* Part 2 */
void* process_transaction(void *arg);
void* update_balance(void *arg);
void setUpAccounts(char *linebuf, size_t *len, FILE *file);
int calcNumTransactions(char *linebuf, size_t *len, FILE *file);
void advanceFilePointerToTransactions(char *linebuf, size_t *len, FILE *file);

/* Part 2 Global Vars */
account *accList;
int numAccs;
int numTrans = 0;
int invalids = 0, deposits = 0, withdraws = 0, transfers = 0, checks = 0;
pthread_t tid[MAX_THREAD];

int main(int argc, char *argv[]){
	size_t len = 0;
	char *linebuf = NULL;
	FILE *file = fopen(argv[1], "r");

	getline(&linebuf, &len, file);
	numAccs = atoi(linebuf);

	accList = (account*)malloc(sizeof(account) * numAccs);

	/* Set up account information  - go through the first portion of the file*/
	setUpAccounts(linebuf, &len, file);

	/* Calculate number of transactions  - go through the rest of the file*/
	numTrans = calcNumTransactions(linebuf, &len, file);
	printf("Num transactions: %d\n", numTrans);
	

	/* Reset file pointer to begining of input file */
	rewind(file);

	/* Move file pointer to be at start of "transactions" portion
	 * of input  file */
	advanceFilePointerToTransactions(linebuf, &len, file);

	/* Save all transactions into a char* array */
	char *allTransactions[numTrans]; 

	for(int i = 0; i < numTrans; i++){
		allTransactions[i] = NULL;
		len = 0;
		getline(&allTransactions[i], &len, file);
	}

	process_transaction(allTransactions);


	/* Now, accept all transaction commands line by line */
	/*
	while(getline(&linebuf, &len, file)!= -1){
		char **linebuf_ptr = &linebuf;
		process_transaction(linebuf_ptr);
	}
	*/




	/* add reward for every account */
	addRewardToBalance(accList, numAccs);
	printBalances(accList, numAccs);

	/* Part 1 */
	free(linebuf);
	free(accList);
	fclose(file);
	/* Part 2 */
	for (int i = 0; i < numTrans; i++){
		free(allTransactions[i]);
	}
}


void* process_transaction(void *arg){

	char **allTransactions = (char**)arg;

	/*
	for (int i = 0; i < numTrans; i++){
		sleep(1);
		printf("Line %d: %s\n", i, allTransactions[i]);
	}
	*/
	for(int i = 0; i < numTrans; i++){
		command_line transaction;
		int verified;
		transaction = str_filler(allTransactions[i], " ");
		
		char action[128];
		char acc[128];
		char pw[128];
		strcpy(action, transaction.command_list[0]);
		strcpy(acc, transaction.command_list[1]);
		strcpy(pw, transaction.command_list[2]);

		verIn ver = verify(numAccs, accList, acc, pw);	

		if(strcmp("D", action) == 0 && ver.verif){
			deposit(ver.index, accList, atof(transaction.command_list[3]));
			deposits++;

		} else if(strcmp("W", action) == 0 && ver.verif){
			withdraw(ver.index, accList, atof(transaction.command_list[3]));
			withdraws++;

		} else if(strcmp("T", action) ==0 && ver.verif){
			transfer(ver.index, accList, atof(transaction.command_list[4]), transaction.command_list[3], numAccs);
			transfers++;

		} else if(strcmp("C", action) ==0 && ver.verif){
			checks++;

		} else if(!ver.verif){
			invalids++;

		} else {
			printf("Extreme error\n");
		}

		free_command_line(&transaction);
	}

}



void* update_balance(void *arg){
	
}



void printAccs(int numAccs, account *accList){
	for(int i = 0; i < numAccs;i++){
		printf("%d acc: %s ", i, accList[i].account_number);
		printf("pw: %s ", accList[i].password);
		printf("bal: %f ", accList[i].balance);
		printf("rw rt: %f ", accList[i].balance);
		printf("tt: %f", accList[i].transaction_tracter);
		printf("\n");
	}

}

verIn verify(int numAccs, account *accList, char *acc, char *pw){
	verIn ret;
	ret.verif = 0;
	for(int i = 0; i < numAccs; i++){
		if(strcmp(accList[i].account_number, acc) == 0 && strcmp(accList[i].password, pw) ==0){
			ret.index = i;
			ret.verif = 1;
		}
	}
	return ret;
}

void deposit(int index, account *accList, double amt){
	accList[index].balance += amt;
	accList[index].transaction_tracter += amt;
}

void withdraw(int index, account *accList, double amt){
	accList[index].balance -= amt;
	accList[index].transaction_tracter += amt;
}

void transfer(int index, account *accList, double amt, char *acc2, int numAccs){
	accList[index].balance -= amt;
	accList[index].transaction_tracter += amt;
	for(int i = 0; i < numAccs; i++){
		if(strcmp(accList[i].account_number, acc2) == 0){
			accList[i].balance += amt;
		}
	}
}

void addRewardToBalance(account *accList, int numAccs){
	for(int i = 0; i < numAccs; i++){
		accList[i].balance += (accList[i].reward_rate * accList[i].transaction_tracter);
	}
}

void printBalances(account *accList, int numAccs){
	for(int i = 0; i < numAccs; i++){
		printf("%d balance: %.2f\n", i, accList[i].balance);
	}
}

void setUpAccounts(char *linebuf, size_t *len, FILE *file){
	printf("First line: %s\n", linebuf);
	for(int i = 0; i < numAccs; i++){
		/* skip the "index" line */
		getline(&linebuf, len, file);

		getline(&linebuf, len, file);
		linebuf[strcspn(linebuf, "\n")] = 0;
		strcpy(accList[i].account_number, linebuf);

		getline(&linebuf, len, file);
		linebuf[strcspn(linebuf, "\n")] = 0;
		strcpy(accList[i].password, linebuf);

		getline(&linebuf, len, file);
		accList[i].balance = atof(linebuf);

		getline(&linebuf, len, file);
		accList[i].reward_rate = atof(linebuf);

		accList[i].transaction_tracter = 0;
	}
}

int calcNumTransactions(char *linebuf, size_t *len, FILE *file){
	int numTrans = 0;

	while(getline(&linebuf, len, file)!= -1){
		numTrans++;
	}
	return numTrans;

}


/* Kind of hacky, but this gets our file pointer back to the start
 * of the transactions portion of our input file */
void advanceFilePointerToTransactions(char *linebuf, size_t *len, FILE *file){
	for(int i = 0; i < numAccs; i++){
		getline(&linebuf, len, file);
		getline(&linebuf, len, file);
		getline(&linebuf, len, file);
		getline(&linebuf, len, file);
		getline(&linebuf, len, file);

	}

	/* We need one more line for some reason */
	getline(&linebuf, len, file);
}
