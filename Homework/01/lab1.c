#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	srand(time(NULL));
	printf("Hello world! This is Introduction to Operating Systems, Spring 2025!\n");
	int randint = rand();
	printf("Boruch Khazanovich %d\n", randint);
}
