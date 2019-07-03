/*  A simple server in the internet domain using TCPThe port number is passed as an argument
	This version runs forever, forking off a separate process for each connection
*/
#include <bits/stdc++.h>
#include <poll.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define CLR "\n\033[2A\033[1B\033[2K\r"
char ack[] = {"Message recieved.\n"};
bool connected = false, start_as_client = false, start_as_host = false, running = true;
int server_port, comm_port, pid, comm_socket, server_socket;
char server_addr_str[ INET_ADDRSTRLEN ], comm_addr_str[ INET_ADDRSTRLEN ];
struct sockaddr_in server_addr, comm_addr;
pthread_t secondary_thread;
fd_set master;
socklen_t comm_addr_len = sizeof(comm_addr);

void *async_read_loop(void *); /* function prototype */
void waitExit(void); /* function prototype */
void error(const char *msg) {
	perror(msg);
}

int main(int argc, char *argv[]) {
	
	atexit(waitExit);
	int n;
	char *buffer = (char *)calloc(1024, sizeof(char));
	string userInput;

	if (argc >= 2) {
		if (strcmp(argv[1], "join") == 0) {
			start_as_client = true;
		} else if (strcmp(argv[1], "host") == 0) {
			start_as_host = true;
		}
	} else {
		error("USAGE: ./player host port\n              or\n       ./player join host_ip host_port\n");
		return 2;
	}

	if(start_as_host) {
		if (argc >= 3) {
			server_port = atoi(argv[2]);
		} else {
			error("USAGE: ./player host port\n");
			return 2;
		}

		if (server_port <= 0) {
			error("ERROR invalid port\n");
			return 2;
		}
		printf("Server configured on port: %d\n", server_port);
	}

	comm_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(comm_socket < 0) {
		error("ERROR opening socket\n");
		return 2;
	}

	if (start_as_client) {
		if (argc >= 3) {
			memset((void *) comm_addr_str, 0, sizeof(comm_addr_str));
			memcpy(comm_addr_str, argv[2], min(strlen(argv[2]) + 1, sizeof(comm_addr_str)));
			printf("%s\n", comm_addr_str);
		} else {
			error("USAGE: ./player join host_ip host_port\n");
			return 2;
		}

		if (argc >= 4) {
			comm_port = atoi(argv[3]);
			if (comm_port <= 0) error("ERROR invalid host_port\n");
		} else {
			error("USAGE: ./player join host_ip host_port\n");
			return 2;
		}

		memset((void *) &comm_addr, 0, sizeof(comm_addr));
		comm_addr.sin_family = AF_INET;
		inet_pton(AF_INET, comm_addr_str, &comm_addr.sin_addr);
		comm_addr.sin_port = htons(comm_port);
		printf("Connecting to server on %s:%d\n", comm_addr_str, comm_port);

		if (connect(comm_socket, (struct sockaddr *) &comm_addr, sizeof(comm_addr)) < 0) {
			error("ERROR connecting\n");
			return 2;
		}
		inet_ntop(AF_INET, &comm_addr, comm_addr_str, INET_ADDRSTRLEN);
		// cout << "after connecting client data " << comm_addr_str << ":" << ntohs(comm_addr.sin_port) << "\n";
		connected = true;
	}

	if((pid = pthread_create(&secondary_thread, NULL, async_read_loop, NULL))) {
		printf("error: pthread_create, id: %d\n", pid);
		return EXIT_FAILURE;
	}

	while(!connected);

	while(true) {
		// cout << CLR << "> ";
		getline(cin, userInput);

		if(!running) break;

		if (userInput.size() > 0) {

			n = write(comm_socket, userInput.c_str(), userInput.size() + 1);
			// printf("client  %-15s : %s\n", server_addr_str, userInput.c_str());
			if (n < 0) error("ERROR writing to socket\n");
		}
		// n = write(write_socket, ack, strlen(ack));
		// if (n < 0) error("ERROR writing to socket");
		// printf("  %-15s : Acknowledge Sent.\n",server_addr_str);
	}
	free(buffer);
	printf("Exiting inside client\n");
	return 0;
}

void *async_read_loop(void *arg) {
	int n, activity;
	FD_ZERO(&master);
	char *buffer = (char *)calloc(1024, sizeof(char));

	if (start_as_host) {
		server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(server_socket < 0) { 
			error("ERROR opening socket\n");
			pthread_exit(NULL);
		}

		memset((void *) &server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(server_port);

		if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
			error("ERROR on binding\n");
			pthread_exit(NULL);
		}
		listen(server_socket, 1);
		inet_ntop(AF_INET, &server_addr, server_addr_str, INET_ADDRSTRLEN);
		cout << "Waiting for client on " << server_addr_str << endl;

		if((comm_socket = accept(server_socket, (struct sockaddr *) &comm_addr, &comm_addr_len)) < 0) {
			error("ERROR on accept\n");
			pthread_exit(NULL);
		}
		FD_SET(server_socket, &master);
		connected = true;
	}

	FD_SET(comm_socket, &master);
	inet_ntop( AF_INET, &comm_addr, comm_addr_str, INET_ADDRSTRLEN );
	comm_port = ntohs(comm_addr.sin_port);

	while(!connected);
	cout << CLR << ">>>> Connection established with client on " << comm_addr_str << " " << comm_port << endl;

	while(true) {
		if(!running) break;

		activity = select(65536, &master, NULL, NULL, NULL);
		if (activity >= 0 && FD_ISSET(comm_socket, &master)) {
			bzero(buffer, 1024);
			n = read(comm_socket, buffer, 1024);
			if (n < 0) error("ERROR reading from socket\n");
			
			cout << CLR << comm_addr_str << " : " << buffer << endl;
			// printf("server read %-15s : %s\n", comm_addr_str, buffer);
		} else {
			printf("No activity on comm_socket: %d\n", comm_port);
		}
	}
	free(buffer);
	pthread_exit(NULL);
}

void waitExit(void) {
	printf("inside waitExit\n");
	pthread_join(secondary_thread, NULL);
	printf("Server thread terminated.\n");
	printf("Closing sockets...\n");
	close(comm_socket);
	close(server_socket);
	printf("Exiting...\n");
}
