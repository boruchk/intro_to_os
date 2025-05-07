#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>


#define NUM_THREADS 4
#define NUM_POINTS 1000000
#define R_MAX 1.0
#define R_MIN -1.0


// Shared variable between threads
int count = 0;
// Thread worker function
void* WorkerThread();
// Semaphore
sem_t sem;


int main(int argc, char* argv[]) {
	if (argc > 1) {
    printf("Usage %s\n", argv[0]);
    return -1;
  }
	
	// Initialize a thread-shared semaphore
	if (sem_init(&sem,	// address of global variable (to be accessed by all threads)
									0,	// allow sharing between threads
									1		// initialize to 1 to act like a mutex
									) != 0) {
		printf("semaphore failed to initialize\n");
		return -1;
	}

	// Array of threads
	pthread_t workers[NUM_THREADS];

	// Make and run the threads with default attributes
	for (int i = 0; i < NUM_THREADS; i++) {
		printf("Starting thread %d\n", i);
		if (pthread_create(&workers[i], NULL, WorkerThread, NULL) != 0) {
			printf("failed to init thread # %d\n", i);
			sem_destroy(&sem);
			return -1;
		}
	}	


	// Wait for (join) all the threads	
	for (int i = 0; i < NUM_THREADS; i++)
		if (pthread_join(workers[i], NULL) != 0) {
			printf("error joining thread #%d\n", i);
			sem_destroy(&sem);
			return -1;
		}

	int total_points = NUM_THREADS * NUM_POINTS;
	double area = 4.0 * ((double)count / (double)total_points);
	printf("--------------------------------\n");
	printf("The area of the circle is: %f\n", area);
	printf("--------------------------------\n");
	
	// Clean up
	sem_destroy(&sem);
	
	return 0;
}


void* WorkerThread() {
	unsigned int rand_state = (unsigned int) time(NULL) + pthread_self();
	
	// Generate NUM_POINTS points
	for (int i = 0; i < NUM_POINTS; i++) {
		// Generate random numbers for x and y in the range of -1 to 1
		double range = R_MIN + (R_MAX - R_MIN);
		double x = range * (double)rand_r(&rand_state) / (RAND_MAX + R_MAX);
		double y = range * (double)rand_r(&rand_state) / (RAND_MAX + R_MAX);

		// Check if the point is within the unit circle
		double r = sqrt(x*x + y*y);
		if (r <= 1) {
			// Get the semaphore to ensure only this thread is incrementing "count"
			sem_wait(&sem);
			++count;			
			// Let go of the semaphore
			sem_post(&sem);
		}
	}
}


