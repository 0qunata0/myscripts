#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef struct {
	int time;
	int employee_id;
	int company_id;
	int operation;
	int amount_of_money;
} operation;

int num_of_threads, td, tw, tb, tin, tout, com1_bal, com2_bal;
int count_ops = 0;
sem_t mutex;
operation *ops;

void deposit(void* oper);
void withdrawal(void* oper);
void balancecheck(void* oper);
void readAndSplitData(char* line);

int main(int argc, char** argv)
{
	if (argc != 2){
		fprintf(stderr, "Usage: %s <input file path>\n", argv[0]);
		exit(1);
	}

	FILE* file = fopen(argv[1], "r");
	char buf[256];
	ops = malloc(sizeof(operation));

	while (fgets(buf, sizeof(buf), file)){
		// Remove newline character by setting the last term to NULL
		buf[strlen(buf) - 1] = '\0';
		// Get the first value
		readAndSplitData(buf);
	}
	printf("Read File successfully.\nM: %d, Td: %d, Tw: %d, Tb: %d, Tin: %d, Tout: %d, Balance of Com1: %d, Balance of Com2: %d\n\n", num_of_threads, td, tw, tb, tin, tout, com1_bal, com2_bal);
	
	fclose(file);
	
	sem_init(&mutex, 0 , 1);
	int i, ret;
	struct threadpool *pool;
	
	if ((pool = threadpool_init(num_of_threads)) == NULL){
		fprintf(stderr, "Failed to create thread pool.\n");
		exit(1);
	}
	
	for (i = 0; i < count_ops; i++){
		if (ops[i].operation == 1)
			ret = threadpool_add_task(pool, balancecheck, (void *) &ops[i], 0);
		else if (ops[i].operation == 2)
			ret = threadpool_add_task(pool, deposit, (void *) &ops[i], 0);
		else
			ret = threadpool_add_task(pool, withdrawal, (void *) &ops[i], 0);
	}

	threadpool_free(pool, 1);
	sem_destroy(&mutex);

	printf("Summary:\nBalance of Company 1: %d\nBalance of Company 2: %d\n", com1_bal, com2_bal);
	return 0;
}

void deposit(void* oper)
{
	operation* op = (operation*) oper;

	sleep(tin);
	sem_wait(&mutex);
	printf("Employee %d deposits %d dollars into Company %d\nPlease wait for %d sec\n\n",
		op->employee_id, op->amount_of_money, op->company_id, td);
	// Critical Section
	sleep(td);
	if (op->company_id == 1){
		com1_bal += op->amount_of_money;
		printf("Company 1 has balance: %d\n\n", com1_bal);
	}
	else{
		com2_bal += op->amount_of_money;
		printf("Company 2 has balance: %d\n\n", com2_bal);
	}
	sem_post(&mutex);
	sleep(tout);
}

void withdrawal(void* oper)
{
	operation* op = (operation*) oper;

	sleep(tin);
	sem_wait(&mutex);
	printf("Employee %d withdraws %d dollars into Company %d\nPlease wait for %d sec\n\n",
		op->employee_id, op->amount_of_money, op->company_id, tw);
	// Critical Section
	sleep(tw);
	if (op->company_id == 1){
		com1_bal -= op->amount_of_money;
		printf("Company 1 has balance: %d\n\n", com1_bal);
	}
	else{
		com2_bal -= op->amount_of_money;
		printf("Company 2 has balance: %d\n\n", com2_bal);
	}
	sem_post(&mutex);
	sleep(tout);
}

void balancecheck(void* oper)
{
	operation* op = (operation*) oper;
	
	sleep(tin);
	sem_wait(&mutex);
	printf("Employee %d check the balance of Company %d\nPlease wait for %d sec\n\n",
		op->employee_id , op->company_id, tb);
	// critical Section
	sleep(tb);
	if (op->company_id == 1)
		printf("Company 1 has balance: %d\n\n", com1_bal);
	else
		printf("Company 2 has balance: %d\n\n", com2_bal);
	sem_post(&mutex);
	sleep(tout);
}

void readAndSplitData(char* line)
{
	int i = 0;
	char* token = strtok(line, " ");

	if (strcmp("M", token) == 0)
		num_of_threads = atoi(strtok(NULL, " "));
	else if (strcmp("Td", token) == 0)
		td = atoi(strtok(NULL, " "));
	else if (strcmp("Tw", token) == 0)
		tw = atoi(strtok(NULL, " "));
	else if (strcmp("Tb", token) == 0)
		tb = atoi(strtok(NULL, " "));
	else if (strcmp("Tin", token) == 0)
		tin = atoi(strtok(NULL, " "));
	else if (strcmp("Tout", token) == 0)
		tout = atoi(strtok(NULL, " "));
	else if (strcmp("Company1", token) == 0){
		while (token != NULL){
			if (i++ == 2){
				memmove(token, token + 1, strlen(token));
				com1_bal = atoi(token);
				break;
			}
			token = strtok(NULL, " ");
		}
	}
	else if (strcmp("Company2", token) == 0){
		while (token != NULL){
			if (i++ == 2){
				memmove(token, token + 1, strlen(token));
				com2_bal = atoi(token);
				break;
			}
			token = strtok(NULL, " ");
		}
	}
	else if (strcmp("Time", token) == 0){
		int position = 0;
		ops = realloc(ops, (count_ops + 1) * sizeof(operation));
		while (token != NULL){
			if (position == 1){
				token[strlen(token) - 1] = '\0';
 				ops[count_ops].time = atoi(token);
			}
			else if (position == 3){
				ops[count_ops].employee_id = atoi(token);
			}
			else if (position == 4){
				ops[count_ops].company_id = (strcmp("Company1;", token) == 0)? 1 : 2;
			}
			else if (position == 5){
				if (strcmp("check", token) == 0){
					ops[count_ops].operation = 1;
					ops[count_ops].amount_of_money = -1;
				}
				else if (strcmp("deposit", token) == 0){
					ops[count_ops].operation = 2;
					token = strtok(NULL, " ");
					memmove(token, token + 1, strlen(token));
					ops[count_ops].amount_of_money = atoi(token);
				}
				else if (strcmp("withdrawal", token) == 0){
					ops[count_ops].operation = 3;
					token = strtok(NULL, " ");
					memmove(token, token + 1, strlen(token));
					ops[count_ops].amount_of_money = atoi(token);
				}
			}
			position++;
			token = strtok(NULL, " ");
		}
		count_ops++;
	}
}
