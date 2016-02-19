#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

void handler (void *ptr);

sem_t mutex;
int counter;

int main()
{
	int i[2];
	pthread_t thread_a;
	pthread_t thread_b;

	i[0] = 0;
	i[1] = 1;

	sem_init(&mutex, 0, 1);

	pthread_create(&thread_a, NULL, (void *) &handler, (void *) &i[0]);
	pthread_create(&thread_b, NULL, (void *) &handler, (void *) &i[1]);

	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

	sem_destroy(&mutex);

	exit(0);
}

void handler (void *ptr)
{
	int x;
	x = *((int *) ptr);

	printf("Thread %d: Waiting to enter critical region...\n", x);
	sem_wait(&mutex);

	printf("Thread %d: Now in critical region...\n", x);
	printf("Thread %d: Counter Value. %d\n", x, counter);
	printf("Thread %d: Incrementing counter...\n", x);
	counter++;
	printf("Thread %d: Now Counter Value: %d\n", x, counter);
	printf("Thread %d: Exiting critical region.\n", x);

	sem_post(&mutex);
	pthread_exit(0);
}
