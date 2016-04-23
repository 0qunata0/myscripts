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

set_t mutex;
int total_num_of_passengers_now = 0;
tag collection_box[MAX_ROW_NUM * MAX_COL_NUM];

void create_a_tag(int col, int row)
{
	static int count = 0;
	collection_box[count].col_num = col;
	collection_box[count].row_num = row;
	collection_box[count++].luggage = 2;
}

void initialize_collection_box()
{
	int i, j;

	// Initialize the collection_box array
	for (i = 0; i <= MAX_COL_NUM; i++){
		for (j = 0; j <= MAX_ROW_NUM; j++)
			create_a_tag(i + 1, j + 1);
	}
}

void* wait_for_passenger(void* passenger)
{
	sleep(rand() % 10);

	sem_wait(&mutex);
	total_num_of_passengers_now++;
	sem_post(&mutex);
}

void* ticketing_machine(void *have_luggage)
{
	static int count = 0;
	sem_wait(&mutex);
	// Enqueue into the collection_box array
	collection_box[count++].luggage = *((int *)have_luggage);
	printf("Running thread\n");
	sleep(rand() % 10);

	sem_post(&mutex);
	return NULL;
}

void create_driver_thread(int num_of_threads)
{
	int i, coff, luggage;
	coff = luggage = 0;
	void* result;
	pthread_t threads[num_of_threads];
	
	initialize_collection_box();

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
}

void create_wating_passengers_thread(int num_of_threads)
{
	int i;
	void* result;
	pthread_t threads[num_of_threads];

	for (i = 0; i < num_of_threads; i++){
		pthread_create(&threads[i], NULL, wait_for_passenger, (void*) &i);
	}
}

int main(int argc, char** argv)
{
	int coming_passengers;
	
	if (argc != 2){
		fprintf(stderr, "Usage: %s [number of people]\n", argv[0]);
		fprintf(stderr, "Example: %s 25\n", argv[0]);
		fprintf(stderr, "In the above example, 25 people will take the couch. 25 threads will be created.\n");
		return 1;
	}

	sem_init(&mutex, 0, 1);
	coming_passengers = atoi(argv[1]);

	create_waiting_passengers_thread(coming_passengers);

	do {
		sem_wait(&mutex);
		if (total_num_of_passengers_now > 36)
			break;
		sem_post(&mutex);
	} while (1);

	for (;;){
		sem_wait(&mutex);
		if (total_num_of_passengers_now % 36 != 0) {
			sem_post(&mutex); continue;
		}

		total_num_of_passengers_now -= 36;
		sem_post(&mutex);

		create_driver_thread(36);
		// Your sleep function
	}

	sem_destroy(&mutex);
	return 0;
}
