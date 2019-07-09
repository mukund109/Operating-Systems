/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "pthread.h"

int MAX_THREADS = 10;

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int sockfd;

void * thread(void * args){
	int fd = (int)args;

	char buffer[256];
	bzero(buffer,256);

	int n = read(fd,buffer,255);
	if (n < 0) error("ERROR reading from socket");
	printf("Here is the message: %s\n",buffer);
	n = write(fd, "I got your message", 18);
	if (n < 0) error("ERROR writing to socket");
	return NULL;
}
int main(int argc, char *argv[])
{
	// opening socket
	int newsockfd, portno;//, clilen;
	unsigned int clilen;
	struct sockaddr_in serv_addr, cli_addr;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	printf("Binding to port %d \n", portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	listen(sockfd,5);
	printf("Listening for connections\n");

	clilen = sizeof(cli_addr);
	char ip[INET_ADDRSTRLEN];
	pthread_t p[MAX_THREADS];
	int num_connections = 0;
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");

		// getting client IP
		struct in_addr ipAddr = cli_addr.sin_addr;
		inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);
		printf("Client details: IP address %s, port number %d \n", ip, ntohs(cli_addr.sin_port));
		
		//spawn new thread to deal with connection
		pthread_create(&p[num_connections], NULL, thread, (void *)newsockfd);
		
		num_connections++;
		if(num_connections==MAX_THREADS){
			printf("Too many connections, closing socket\n");
			close(sockfd);
			for(int i =0; i<MAX_THREADS; i++)
				pthread_join(p[i], NULL);
			printf("Shutting down server\n");
			return 0;
		}
		// try out pthread_tryjoin_np to get return value from thread
		
	}

	return 0; 
}
