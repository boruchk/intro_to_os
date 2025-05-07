/*
	lab9_B2.c
	Consumer

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
	Consumer dequeues and prints from a circular shared memory buffer
*/
void deque(void* shm, void* shm_mtdt) {
	// Get the same in and out values as enque
	int* inptr = ((int*)shm_mtdt);
	int* outptr = ((int*)shm_mtdt+1);

	// If inptr == outptr then stay in the loop until inptr changes (wait until in != out)
	while (*inptr == *outptr) inptr = ((int*)shm_mtdt);
	// Read from the buffer at the address of the shm+outptr
	printf("%d\n", *((int*)shm+(*outptr)));
	// Increment outptr and store it in shm_mtdt[1]
	*outptr = (*outptr + 1) % BUF_SZ;
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

	// Obtain the pointer to the shared memory
	void* shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) { 
		perror("failed mmap for shm");
		close(shm_fd);
		return -1;
	}


	// Shared memory for inptr, outptr of the shared memory buffer 
	const int SHM_MTDT_SIZE = 2*sizeof(int*);
	const char* SHM_MTDT_NAME = "lab9_shm_mtdt";

	// Open the shared memory metadata virtual file
	int shm_mtdt_fd = shm_open(SHM_MTDT_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_mtdt_fd < 0) {
		perror("failed mmap for shm_mtdt");
		return -1;
	}

	// Obtain the pointer to said buffer
	void* shm_mtdt_ptr = mmap(0, SHM_MTDT_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_mtdt_fd, 0);
	if (shm_mtdt_ptr == MAP_FAILED) {
		perror("failed mmap for shm_mtdt");
		close(shm_mtdt_fd);
		return -1;
	}

	
	// Get input values from cli
	int n = atoi(argv[1]);
	int d = atoi(argv[2]);
	
	printf("start address of shared memory buffer from consumer side: %p\n", shm_ptr);
	printf("address of <n> from consumer side: %p\n", &n);
	// Print contents of shared memory as they come into the shared memory buffer (shm_fd)
	for (int i = 0; i < n; i++) {
		deque(shm_ptr, shm_mtdt_ptr);
	}

	return 0;
}
