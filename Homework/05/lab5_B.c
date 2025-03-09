#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>


int main(int argc, char* argv[]) {
	if (argc < 3) return -1;
	

	// Get input values from cli
	int n = atoi(argv[1]);
	int d = atoi(argv[2]);
	

	// Array to store the read and write fd of the pipe
	int pipe_fd[2];
	if (pipe(pipe_fd) < 0) return -1;
	

	pid_t pid = fork();
	if (pid < 0) return -1;

	
	// Child:
	if (pid == 0) {
		srand(time(NULL));
		// Sleep for random amount of time: [0-3) seconds, and write the result to the pipe
		for (int i = 0; i < n; i++){
			int randval = rand() % 4;
			sleep(randval);
			int result = i*d;
			write(pipe_fd[1], &result, sizeof(result));
		}
		// Close the write end of the pipe
		close(pipe_fd[1]);
	}
	
	// parent:
	else {
		int temp;
		// Print contents of pipe as they come in, writing it to buf+i
		for (int i = 0; i < n; i++) {
			read(pipe_fd[0], &temp, sizeof(int));
			printf("%d\n", temp);
		}
		wait(NULL);
		// Close the read end of the pipe
		close(pipe_fd[0]);
	}

	return 0;
}
