#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define MAX_ROW_NUM 4
#define MAX_COL_NUM 9

using namespace std;

typedef struct
{
	int row_num;
	int col_num;
	int luggage; // 0 to represent Y, 1 to represent N 
} tag;

sem_t mutex;
int total_num_of_passengers_now = 0;
tag collection_box[MAX_ROW_NUM * MAX_COL_NUM];

void create_a_tag(int col, int row)
{
	static int count = 0;
	collection_box[count].col_num = col;
	collection_box[count].row_num = row;
	collection_box[count++].luggage = 1;
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
	cout<<"Running thread\n";
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
		cout<<"Col: "<<collection_box[i].col_num<<	"Row: "<<collection_box[i].row_num<<" Lugguage: "<<collection_box[i].luggage<<"\n";
}

void create_waiting_passengers_thread(int num_of_threads)
{
	int i;
	void* result;
	pthread_t threads[num_of_threads];

	for (i = 0; i < num_of_threads; i++){
		pthread_create(&threads[i], NULL, wait_for_passenger, (void*) &i);
	}
}

int main()
{
	int coming_passengers;
	
	cout<< ">coach " ;
	cin>>coming_passengers;
	cout<<"test";
	sem_init(&mutex, 0, 1);

	create_waiting_passengers_thread(coming_passengers);

	do {
		sem_wait(&mutex);
		if (total_num_of_passengers_now > 36)
			break;
		sem_post(&mutex);
		sleep(2);
		cout<<"testing";
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
	
}
