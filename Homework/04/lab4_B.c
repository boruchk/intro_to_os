#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int subroutine() {
	int pid1 = -2, pid2 = -2, pid3 = -2;

	pid1 = fork();					// make the 1st child
	
	if (pid1 == 0) { 				// in 1st child
		//printf("in 1st child\n");
		return 1;
	}

	else if (pid1 > 0) { 		// in the parent
		pid2 = fork();				// make the 2nd child

		if (pid2 == 0) {			// in the 2nd child
			pid3 = fork();			// make the 3rd child

			if (pid3 == 0) {		// in the 3rd child
				//printf("in 3rd child\n");
				return 3;
			}
			else {
				wait(NULL);				// wait for the 3rd child to exit
				//printf("in 2nd child\n");
				return 2;
			}
		}
		else {
			wait(NULL);					// wait for the 1st child to exit
			wait(NULL);					// wait for the 2nd child to exit
			//printf("in parent\n");
			return 0;
		}
	}
}