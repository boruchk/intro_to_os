#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("Usage %s <n>\n", argv[0]);
		return 1;
	}

	pid_t pid = fork();

	if (pid < 0) {
		printf("fork failed");
		return 1;
	}

	long n = atol(argv[1]);
	long d = atol(argv[2]);

	if (pid == 0) {
		// in child process
		//printf("in child.\nprinting...\n");
		for (int i = 0; i < n; i++) {
			printf("%ld ", d*i);
		}
		//printf("\n");
		exit(0);
	}

	else {
		// in the parent process
		//printf("in parent waiting for child\n");
		wait(NULL);
		//printf("in parent, done waiting for child.\nprinting...\n");
		for (int i = n; i < n+3; i++) {
			printf("%ld ", d*i);
		}
		printf("\n");
	}
	return 0;
}
