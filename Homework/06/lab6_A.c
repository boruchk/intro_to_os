#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>

#define LISTEN_BACKLOG 2
#define SERVER_WAIT_TIME 5
#define PORT 5000


int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("Usage %s <n> (>1) <d>\n", argv[0]);
		return -1;
	}	

	// Get input values from cli
	int n = atoi(argv[1]);
	if (n < 1) {
		printf("Usage %s <n> (>1)  <d>\n", argv[0]);
		return -1;
	}
	int d = atoi(argv[2]);
	

	pid_t pid = fork();
	if (pid < 0) { 
		printf("fork failed\n");
		return -1;
	};

	
	// child/producer/client:
	if (pid == 0) {
		// Make a client socket
		int client_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (client_fd < 0) {
			printf("client socket() call failed\n");
			return -1;
		}

		// Make a struct to remake the server_addr
		// Set the server ip address to 127.0.0.1, and the port 5000 (with bytes set to network order)
		struct sockaddr_in  server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		server_addr.sin_port = htons(PORT);



		// Try to connect to the server (parent) every 0.1s until timeout
		bool connected = false;
		float itter = 0.0;
		printf("Client waiting on server...\n");
		while (!connected) {
			if (connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
				printf("Client successfully connected to server\n");
				connected = true;
			}
			else usleep(100000);
			if (itter > SERVER_WAIT_TIME + 2) {
				printf("connection request from client timed out :(\n");
				exit(-1);
			};
			itter += 0.1;
		}

		// Wait for 0-3 seconds and write the result to the socket
		srand(time(NULL));
		for (int i = 0; i < n; i++){
			int crandval = rand() % 4;
			sleep(crandval);
			int result = i*d;
			write(client_fd, &result, sizeof(result));
		}

		close(client_fd);
		exit(0);
	}
	
	// parent/consumer/server:
	else {
		// wait for 1-5 seconds before starting to listen and accept connections.
		srand(time(NULL) + getpid());
		int randval = (rand() % SERVER_WAIT_TIME) + 1;
		printf("Server waiting for %d seconds...\n", randval);
		sleep(randval);

		
		// Create the socket with TCP protocol
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd < 0) {
			printf("server socket() call failed in main\n");
			return -1;
		}
		
		// Set the server ip address to anything, and the port 5000 (with bytes set to network order)
		struct sockaddr_in  server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(PORT);
		

		// bind the server socket to the server ip address and port where it will begin listening
		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
			printf("error while binding\n");
			return -1;
		}
		
		// listen for a connection request from a client (the child process)
		if (listen(server_fd, LISTEN_BACKLOG) == -1) {
			printf("error while listening\n");
			return -1;
		}


		// Make a client_addr struct to get the information from accept
		// Set the client ip address to 127.0.0.1, and the port 5000 (with bytes set to network order)
		struct sockaddr_in client_addr;
		client_addr.sin_family = AF_INET;
		socklen_t client_sock_addr_len = sizeof(&client_addr);
		client_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		
		// Accept the connection and get the client ip and port from the listening socket
		int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_sock_addr_len);
		if (client_fd == -1) {
			printf("error while accepting\n");
			return -1;
		}
		printf("Server accepted connection from client\n");


		// Read the data from the socket
		int temp;
		for (int i = 0; i < n; i++) {
			read(client_fd, &temp, sizeof(temp));
			printf("%d\n", temp);
			fflush(stdout);
		}
		
		// Wait for the child to exit and close the socket
		wait(NULL);
		close(server_fd);
	}

	return 0;
}
