#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_parser.h"
#include "account.h"

/* the return of the verify command, returns status as well as index */
/*
typedef struct{
	int verified;
	int index;
}verAndIndex;
*/

/*
void deposit(int index, account *accList, command_line *command);
void withdraw(int index, account *accList, command_line *command);
void transfer(int accNum, int index, account *accList, command_line *command);
void caclualteReward(int accNum, account *accList);
void printBalances(int accNum, account *accList);
*/

/*
verAndIndex verifyAccount(int accNum, account *accList, char *acc, char *pw);
*/

int verifyAccount(int accNum, account *accList, char *acc, char *pw);
int main(int argc, char*argv[]){

	/* variables */
	size_t len = 128;
	char * linebuf = (char*)malloc(len);
	FILE *file = fopen(argv[1], "r");
	char *accNumStr = (char*)malloc(200);
	int accNum;
	account *accList = (account*)malloc(sizeof(account) * (accNum));
	
	/* Get first line, number of accounts */
	getline(&linebuf, &len, file);
	strcpy(accNumStr, linebuf);
	accNum = atoi(accNumStr);

	/* Fill account structs with data */
	int lineNum = 1;
	int accLine = 0;
	int currAcc = 0;
	char *useless_save;

	while(getline(&linebuf, &len, file) != -1){
		/* Holder variables so we can convert these to double before saving
		 * in the account struct */
		char *balance_str = (char*)malloc(300);
		char *rwd_str = (char*)malloc(300);

		/* Account number */
		if(accLine ==1){
			linebuf = strtok_r(linebuf, "\n", &useless_save);
			strcpy(accList[currAcc].account_number, linebuf);
			/*
			printf("Acc cumber: %s\n", accList[currAcc].account_number);
			*/
		}

		/* Account password */
		else if(accLine == 2){
			linebuf = strtok_r(linebuf, "\n", &useless_save);
			strcpy(accList[currAcc].password, linebuf);
			/*
			printf("pw: %s\n", accList[currAcc].password);
			*/
		}

		/* Account initial balance */
		else if(accLine ==3){
			strcpy(balance_str, linebuf);
			accList[currAcc].balance = atof(balance_str);
			/*
			printf("balance: %f\n", accList[currAcc].balance);
			*/

		}
		/* Account reward rate */
	       	else if(accLine == 4){
			strcpy(rwd_str, linebuf);
			accList[currAcc].reward_rate = atof(rwd_str);
			accLine = -1;
			currAcc++;
		}	
		accList[currAcc].transaction_tracter = 0;
		lineNum++;
		accLine++;
		free(balance_str);
		free(rwd_str);
		if(lineNum >=( accNum * 5)+1){
			break;
		}
	}

	/* Preform all account actions */
	int invalidPws = 0, accNotFounds = 0, checks = 0, withdraws = 0, transfers = 0, deposits = 0;
	int holder = 0;
	while(getline(&linebuf, &len, file) != -1&&holder <=10){
		/*
		verAndIndex verIn;
		*/
		int verified;
		int index;

		command_line command;
		command = str_filler(linebuf, " ");	

		char *action = (char*)malloc(100);
		char *acc = (char*)malloc(300);
		char *pw = (char*)malloc(300);


		strcpy(action, command.command_list[0]);
		strcpy(acc, command.command_list[1]);
		strcpy(pw, command.command_list[2]);

		verified = verifyAccount(accNum, accList, acc, pw);
		/*
		verified = verIn.verified;
		index = verIn.index;
		*/

		/*
		printf("Action: %s  ", action);
		printf("Acc: %s ", acc);
		printf("pw: %s ", pw);
		printf("Verif: %d ", verified);
		*/


		/* Deposit */
		if(strcmp(action, "D")==0 && verified==0){
			/*
			deposit(index, accList, &command);
			*/

		} 

		/* Withdraw */
		else if(strcmp(action, "W")==0 && verified ==0){
			/*
			withdraw(index, accList, &command);
			*/

		}

		/* Transfer */
		else if(strcmp(action, "T")==0 && verified == 0){


		}
		/* Check balance */
		else if(strcmp(action, "C")==0 && verified ==0){

		}
		else if(verified ==1){
			invalidPws++;
		} else if(verified ==2){
			accNotFounds++;
		} else {
			printf("Extremely strange error: ya blew it ");
		}

	        free_command_line(&command);	
		free(acc);
		free(pw);
		free(action);
		holder++;
	}

	free(linebuf);
	free(accList);
	free(accNumStr);
	fclose(file);
}

/*
void deposit(int index, account *accList, command_line *command){
	int amt = atof(command->command_list[3]);
	accList[index].balance += amt; 
	accList[index].transaction_tracter += amt; 
	
}

void withdraw(int index, account *accList, command_line *command){
	int amt = atof(command->command_list[3]);
	accList[index].balance -= amt; 
	accList[index].transaction_tracter -= amt; 
	
}

void transfer(int accNum, int index, account *accList, command_line *command){
	int amt = atof(command->command_list[4]);
	accList[index].balance -= amt;
	accList[index].transaction_tracter += amt;

	for(int i =0; i < accNum; i++){
		if(strcmp(accList[i].account_number, command->command_list[3]) == 0){
			accList[i].balance += amt;
		} else {
			printf("Error: could not find recipient account! ");
		}
	}
}
*/

/* verified: 0 if good, 1 if wrong password, 2 if account not even found at all 
 * index: where in accList we can find this account */


int verifyAccount(int accNum, account *accList, char *acc, char *pw){

	/*
	verAndIndex ret;
	ret.verified = 2;	
	*/

	int ret = 2;
	for(int i =0; i < accNum; i++){

		if(strcmp(accList[i].account_number, acc) == 0){
			/*
			ret.verified = 1;
			*/
			ret = 1;

			if(strcmp(accList[i].password, pw)==0){
				/*
				ret.verified = 0;
				ret.index = i;
				*/
				ret = 0;
			}
		}
	}
	return ret;
}

/*
void caclualteReward(int accNum, account *accList){
	for(int i = 0; i < accNum; i++){
		accList[i].balance += (accList[i].reward_rate * accList[i].transaction_tracter);
	}

}
void printBalances(int accNum, account *accList){
	for(int i = 0; i < accNum; i++){
		printf("%d balance: %f\n", i, accList[i].balance);
	}

}
*/

