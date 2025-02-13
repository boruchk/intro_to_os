#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


int main(int argc, char* argv[]) {
	// Console log to determine where program begins while using strace
	printf("mycat.c starts here:\n");
	// Check that the user correctly passes a file location to the program
	if (argc != 2) {
		printf("input a file please!");
		exit(-1);
	}
	
	id_t pid = getpid();
	printf("The process id is: %d\n", pid);
	
	// Seed the random value generator
	srand(time(NULL));
	int randint = rand() % (5);
	
	printf("sleepint for %d seconds after printing pid\n\n", randint);
	sleep(randint);

	// Get the file name from the array of arguments (argv)
	char* filename = argv[1];
	
	// Get the file descriptor (fd) from the open() sys call and check for file open error
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("error while opening file\n");
		exit(1);
	}
	
	// Allocate some memory (100 bytes) to store the text in the input file
	char* text = (char*)calloc(100, sizeof(char));
	size_t bytestoread = 100;
	// read the file and check for read error
	ssize_t bytesread = read(fd, text, bytestoread);
	if (bytesread < 0) {
		printf("error while reading file\n");
		exit(1);
	}	
	
	printf("%zu bytes were read\n\n", bytesread);
	// Print the contents of the file from the buffer
	printf("The text in filedescriptor %d is:\n", fd);
	printf("%s\n", text);
	
	close(fd);

	return 0;
}
