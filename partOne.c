#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_parser.h"
#include "account.h"

typedef struct{
	int verif;
	int index;
}verIn;

void printAccs(int numAccs, account *accList);

void withdraw(int index, account *accList, double amt);
void deposit(int index, account *accList, double amt);
void transfer(int index, account *accList, double amt, char *acc2, int numAccs);
void addRewardToBalance(account *accList, int numAccs);
void printBalances(account *accList, int numAccs);

verIn verify(int numAccs, account *accList, char *acc, char *pw);

int main(int argc, char *argv[]){
	size_t len = 0;
	char *linebuf = NULL;
	FILE *file = fopen(argv[1], "r");
	int numAccs;

	getline(&linebuf, &len, file);
	numAccs = atoi(linebuf);

	account *accList = (account*)malloc(sizeof(account) * numAccs);

	/* Set up account information */
	for(int i = 0; i < numAccs; i++){
		getline(&linebuf, &len, file);

		getline(&linebuf, &len, file);
		linebuf[strcspn(linebuf, "\n")] = 0;
		strcpy(accList[i].account_number, linebuf);

		getline(&linebuf, &len, file);
		linebuf[strcspn(linebuf, "\n")] = 0;
		strcpy(accList[i].password, linebuf);

		getline(&linebuf, &len, file);
		accList[i].balance = atof(linebuf);

		getline(&linebuf, &len, file);
		accList[i].reward_rate = atof(linebuf);

		accList[i].transaction_tracter = 0;
	}

	/* Now, accept all transaction commands line by line */
	int invalids = 0, deposits = 0, withdraws = 0, transfers = 0, checks = 0;
	while(getline(&linebuf, &len, file)!= -1){
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

	/* add reward for every account */
	addRewardToBalance(accList, numAccs);
	printBalances(accList, numAccs);

	free(linebuf);
	free(accList);
	fclose(file);
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
