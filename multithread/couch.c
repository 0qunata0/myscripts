#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_ROW_NUM 4
#define MAX_COL_NUM 9

typedef struct
{
	int row_num;
	int col_num;
	int luggage; // 0 to represent Y, 1 to represent N and 2 to represent E
} tag;

tag collection_box[MAX_ROW_NUM * MAX_COL_NUM];
pthread_mutex_t ticket_lock = PTHREAD_MUTEX_INITIALIZER;

void create_a_tag(int col, int row)
{
	static int count = 0;
	collection_box[count].col_num = col;
	collection_box[count].row_num = row;
	collection_box[count++].luggage = 2;
}

void* ticketing_machine(void *have_luggage)
{
	static int count = 0;
	pthread_mutex_lock(&ticket_lock);
	// Enqueue into the collection_box array
	collection_box[count++].luggage = *((int *)have_luggage);
	//sleep(rand() % 60);

	pthread_mutex_unlock(&ticket_lock);
	return NULL;
}

int main(int argc, char** argv)
{
	void* result;
	int i, j, num_of_threads, coff, luggage;
	i = j = coff = luggage = 0;
	
	if (argc != 2){
		fprintf(stderr, "Usage: %s [number of people]\n", argv[0]);
		fprintf(stderr, "Example: %s 25\n", argv[0]);
		fprintf(stderr, "In the above example, 25 people will take the couch. 25 threads will be created.\n");
		return 1;
	}

	num_of_threads = atoi(argv[1]);
	pthread_t threads[num_of_threads];

	// Initialize the collection_box array
	for (i = 0; i <= MAX_COL_NUM; i++){
		for (j = 0; j <= MAX_ROW_NUM; j++)
			create_a_tag(i + 1, j + 1);
	}

	for (i = 0; i < num_of_threads; i++){

		if ((i % 5) == 0)
			coff = (rand() % 5) + i;

		luggage = (coff == i) ? 0 : 1 ;

		pthread_create(&threads[i], NULL, ticketing_machine, (void*) &luggage);
	}

	for (i = 0; i < num_of_threads; i++){
		pthread_join(threads[i], &result);
	}

	for (i = 0; i < MAX_COL_NUM * MAX_ROW_NUM; i++)
                printf("Col: %d, Row: %d, Lugguage: %d\n", collection_box[i].col_num, collection_box[i].row_num, collection_box[i].luggage);

	return 0;
}
