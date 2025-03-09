#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUF_SZ 5


void enque(void* buf, void* other_buf, int n) {
	// Get the in and out values (ranges from 0 - BUF_SZ)
	int* inptr = ((int*)other_buf);
	int* outptr = ((int*)other_buf+1);
	
	// Wait until outptr increases 
	while (((*inptr + 1) % BUF_SZ) == *outptr) outptr = ((int*)other_buf+1);
	// Now that outptr != inptr, put the given value (n) into the buffer
	*((int*)buf+(*inptr)) = n;
	// Increment inptr and store it in other_buf[0]
	*inptr = (*inptr + 1) % BUF_SZ;
}


void deque(void* buf, void* other_buf) {
	// Get the same in and out values as enque
	int* inptr = ((int*)other_buf);
	int* outptr = ((int*)other_buf+1);

	// If inptr == outptr then stay in the loop until inptr changes (wait until in != out)
	while (*inptr == *outptr) inptr = ((int*)other_buf);
	// Read from the buffer at the address of the buffer+outptr
	printf("%d\n", *((int*)buf+(*outptr)));
	// Increment outptr and store it in other_buf[1]
	*outptr = (*outptr + 1) % BUF_SZ;
}



int main(int argc, char* argv[]) {
	if (argc < 3) return -1;
	
	// Shared memory for circular buffer implementation
	const int MEM_SIZE = BUF_SZ*sizeof(int);
	const char* MEM_NAME = "SHARED_MEMORY";

	int shm_fd = shm_open(MEM_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_fd < 0) return -1;
	if (ftruncate(shm_fd, MEM_SIZE) < 0) return -1;

	void* shm_ptr = mmap(0, MEM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) return -1;


	// Shared memory for inptr, outptr of the circular buffer
	const int OTHER_MEM_SIZE = 2*sizeof(int*);
	const char* OTHER_MEM_NAME = "OTHER_SHARED_MEMORY";

	int other_shm_fd = shm_open(OTHER_MEM_NAME, O_CREAT | O_RDWR, 0666);
	if (other_shm_fd < 0) return -1;
	if (ftruncate(other_shm_fd, OTHER_MEM_SIZE) < 0) return -1;

	void* other_shm_ptr = mmap(0, OTHER_MEM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, other_shm_fd, 0);
	if (other_shm_ptr == MAP_FAILED) return -1;

	// Set inptr and outptr to 0 to start
	*((int*)other_shm_ptr) = 0;
	*((int*)other_shm_ptr+1) = 0;

	
	// Get input values from cli
	int n = atoi(argv[1]);
	int d = atoi(argv[2]);
	

	pid_t pid = fork();
	if (pid < 0) return -1;

	// child:
	if (pid == 0) {
		srand(time(NULL));
		// Sleep for random amount of time: [0-3) seconds, and enque the result into shm_ptr (buf)
		for (int i = 0; i < n; i++){
			int val = rand() % 4;
			//printf("sleeping for _%d_ second(s) in the child process before generating next result...\n", val);
			sleep(val);
			int result = i*d;
			enque(shm_ptr, other_shm_ptr, result);
		}
	}
	
	// parent:
	else {
		// Print contents of shared memory as they come into the shared memory buffer (shm_fd)
		for (int i = 0; i < n; i++) {
			deque(shm_ptr, other_shm_ptr);
		}
		wait(NULL);
		shm_unlink(MEM_NAME);
		shm_unlink(OTHER_MEM_NAME);
		munmap(shm_ptr, MEM_SIZE);
		munmap(other_shm_ptr, OTHER_MEM_SIZE);
		close(shm_fd);
		close(other_shm_fd);
	}

	return 0;
}
