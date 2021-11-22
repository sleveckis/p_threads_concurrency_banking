#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_parser.h"
#include "account.h"
#include <pthread.h>

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

/* Part 2 Global Vars */
account *accList;
int numAccs;
int invalids = 0, deposits = 0, withdraws = 0, transfers = 0, checks = 0;
pthread_t tid[MAX_THREAD];

int main(int argc, char *argv[]){
	size_t len = 0;
	char *linebuf = NULL;
	FILE *file = fopen(argv[1], "r");

	getline(&linebuf, &len, file);
	numAccs = atoi(linebuf);

	accList = (account*)malloc(sizeof(account) * numAccs);

	/* Set up account information */
	setUpAccounts(linebuf, &len, file);
	
	/* Now, accept all transaction commands line by line */
	while(getline(&linebuf, &len, file)!= -1){
		char **linebuf_ptr = &linebuf;
		process_transaction(linebuf_ptr);
	}

	/* add reward for every account */
	addRewardToBalance(accList, numAccs);
	printBalances(accList, numAccs);

	free(linebuf);
	free(accList);
	fclose(file);
}


void* process_transaction(void *arg){
	/*  transform double void pointer down to
	 *  single char pointer, and tokenize the
	 *  string with str_filler */
	char **linebuf_ptr = (char**)arg;
	char *linebuf = *linebuf_ptr;
	command_line transaction;
	int verified;
	transaction = str_filler(linebuf, " ");
	
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

	for(int i = 0; i < numAccs; i++){
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
