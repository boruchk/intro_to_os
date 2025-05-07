#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define FILE_PATH "/dev/lab8"

static bool first_open = true;
static bool first_read = true;


int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage %s <num_bytes>\n", argv[0]);
		return 1;
	}

	// Get the current time
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
		perror("error getting time before open");
		return 1;
	}	
	long time_before_open = ts.tv_nsec;
	long time_after_first_read = 0;
	long time_diff_ms = 0;

	// Open the file
	int fd = open(FILE_PATH, O_RDONLY);
	if (fd < 0) {
		perror("failed to open");
		return 1;
	}
	
	/*
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	printf("wasting cpu time...\n");
	*/

	// Read from the file
	size_t num_bytes = atoi(argv[1]);
	char result[num_bytes + 1];
	memset(result, 0, sizeof(result));
	
	int bytes_read = read(fd, result, num_bytes); 
	if (bytes_read < 0) {
		perror("error reading file");
		return 1;
	}
	else if (bytes_read == 0) printf("end of file\n");

	
	// Get time after first read
	if (first_read) {
		if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
			perror("error getting time before open");
			return 1;
		}
		time_after_first_read = ts.tv_nsec;
		first_read = false;
	}
	time_diff_ms = (time_after_first_read - time_before_open) / 1000000;

	printf("driver counted _ ms to read with _ pid:\n");
	printf("%s\n", result);
	printf("program counted %ldms to read\n", time_diff_ms);

	// test multiple reads
	/*
	for (int i = 0; i < num_bytes*5; i++) {
		int res = read(fd, result, num_bytes);
		if (res == 0) printf("end of file\n");
		else if (res < 0) {
			perror("error reading file");
			break;
		}
		else printf("%s\n", result);
	}
	*/
	return 0;
}
