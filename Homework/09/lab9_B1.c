/*
	lab9_B1.c
	Producer

	cs3224
	04/24/2025
	
	Boruch Khazanovich
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define BUF_SZ 5
#define SHM_NAME "lab9_shm"


/*
	@param shm: shared memory buffer
	@param shm_mtdt: shared buffer for shm metadata
	@param n: value to enque to shm
	Producer enqueues to the shared memory buffer
*/
void enque(void* shm, void* shm_mtdt, int n) {
	// Get the in and out values (ranges from 0 - BUF_SZ)
	int* inptr = ((int*)shm_mtdt);
	int* outptr = ((int*)shm_mtdt+1);
	
	// Wait until outptr increases 
	while (((*inptr + 1) % BUF_SZ) == *outptr) outptr = ((int*)shm_mtdt+1);
	// Now that outptr != inptr, put the given value (n) into the shared mem
	*((int*)shm+(*inptr)) = n;
	// Increment inptr and store it in shared mem metadata[0]
	*inptr = (*inptr + 1) % BUF_SZ;
}


int n = 1;

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("usage %s <n> <d>\n", argv[0]);
		return -1;
	}

	// Shared memory for circular buffer implementation
	const int SHM_SIZE = BUF_SZ*sizeof(int);

	// Open the shared memory (virtual file)
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_fd < 0) {
		perror("failed to open shared mem");
		return -1;
	}
	
	// Truncate the shared memory
	if (ftruncate(shm_fd, SHM_SIZE) < 0) { 
		perror("failed to truncate shm");
		close(shm_fd);
		return -1;
	}

	// Obtain the pointer to the shared memory
	void* shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) { 
		perror("failed mmap for shm");
		close(shm_fd);
		return -1;
	}


	// Shared memory for inptr, outptr of the shared memory buffer (its metadata) 
	const int SHM_MTDT_SIZE = 2*sizeof(int*);
	const char* SHM_MTDT_NAME = "lab9_shm_mtdt";

	// Open the shared memory metadata virtual file
	int shm_mtdt_fd = shm_open(SHM_MTDT_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_mtdt_fd < 0) {
		perror("failed mmap for shm_mtdt");
		return -1;
	}

	// Truncate the shared memory metatdata buffer
	if (ftruncate(shm_mtdt_fd, SHM_MTDT_SIZE) < 0) {
		perror("failed ftruncate for shm_mtdt");	
		close(shm_mtdt_fd);
		return -1;
	}

	// Obtain the pointer to said buffer
	void* shm_mtdt_ptr = mmap(0, SHM_MTDT_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_mtdt_fd, 0);
	if (shm_mtdt_ptr == MAP_FAILED) {
		perror("failed mmap for shm_mtdt");
		close(shm_mtdt_fd);
		return -1;
	}

	// Set inptr and outptr to 0 to start
	*((int*)shm_mtdt_ptr) = 0;
	*((int*)shm_mtdt_ptr+1) = 0;

	
	// Get input values from cli
	int n = atoi(argv[1]);
	int d = atoi(argv[2]);


	// Producer
	printf("start address of shared memory buffer from producer side: %p\n", shm_ptr);
	printf("address of <n> from producer side: %p\n", &n);
	srand(time(NULL));
	// Sleep for random amount of time: [0-3) seconds, and enque the result into shm_ptr (buf)
	for (int i = 0; i < n; i++){
		int val = rand() % 4;
		//printf("sleeping for _%d_ second(s) in the producer process before generating next result...\n", val);
		sleep(val);
		int result = i*d;
		enque(shm_ptr, shm_mtdt_ptr, result);
	}
	
	shm_unlink(SHM_NAME);
	shm_unlink(SHM_MTDT_NAME);
	munmap(shm_ptr, SHM_SIZE);
	munmap(shm_mtdt_ptr, SHM_MTDT_SIZE);
	close(shm_fd);
	close(shm_mtdt_fd);

	return 0;
}
