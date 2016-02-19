#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int time;
	int employee_id;
	int company_id;
	int operation;
	int amount_of_money;
} operation;

int num_of_threads, td, tw, tb, tin, tout, com1_bal, com2_bal;

void* deposit(void* amount);
void* withdrawal(void* amount);
void* balancecheck();
void readAndSplitData(char* buffer);

int main(int argc, char** argv)
{
	if (argc != 2){
		fprintf(stderr, "Usage: %s <input file path>\n", argv[0]);
		exit(1);
	}

	FILE* file = fopen(argv[1], "r");
	char buf[256];
	char* token;
	//char* str_arr[1500];

	while (fgets(buf, sizeof(buf), file)){
		// Remove newline character by setting the last term to NULL
		buf[strlen(buf) - 1] = '\0';
		// Get the first value
		//token = strtok(buf, " ");
		readAndSplitData(buf);
		
	}
	printf("M: %d, Td: %d, Tw: %d, Tb: %d, Tin: %d, Tout: %d, Balance of Com1: %d, Balance of Com2: %d\n", num_of_threads, td, tw, tb, tin, tout, com1_bal, com2_bal);
	return fclose(file);
}

void readAndSplitData(char* buffer)
{
	int i = 0;
	char* token = strtok(buffer, " ");
	operation **ops = NULL;


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
		while (token != NULL && i < 3){
			if (i++ == 2){
				memmove(token, token + 1, strlen(token));
				com1_bal = atoi(token);
				break;
			}
			token = strtok(NULL, " ");
		}
	}
	else if (strcmp("Company2", token) == 0){
		while (token != NULL && i < 3){
			if (i++ == 2){
				memmove(token, token + 1, strlen(token));
				com2_bal = atoi(token);
				break;
			}
			token = strtok(NULL, " ");
		}
	}
	else if (strcmp("Time", token) == 0){
		while (token != NULL){
			ops = (operation **) realloc(ops, (++i) * sizeof(operation));
			ops[i] = (operation *) malloc(sizeof(operation));
			ops[i]->time = 1;
			ops[i]->employee_id = 1;
		}
	}
}
