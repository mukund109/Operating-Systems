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
#include "buffer.h"
#include <string.h>
#include "hashtable.h"

void error(char *msg)
{
	perror(msg);
	exit(1);
}

//TODO: remove this
pthread_mutex_t stdout_lock;

// listening socket file desc
int sockfd;

// shared hashtable
HashTable * table;
int CAPACITY = 1000;

char * process_request(char * request){
	char * k = strndup(request+1, NUM_DIGITS_IN_KEY);
	int key = atoi(k);
	free(k);
	char * response = NULL;
	if(request[0]=='s'){
		insert(table, key, request+1+NUM_DIGITS_IN_KEY);
		response = strdup("0");
	}
	else if(request[0]=='d'){
		delete_entry(table, key);
		response = strdup("0");
	}
	else if (request[0]=='g'){
		if((response=get(table, key))==NULL)
			response = strdup("1");
	}
	else response = strdup("1");

	printf("\nTABLE:\n");
	print_table(table);
	return response;
}



void * thread(void * args){
	int fd = (int)args;
	
	char buffer[WRITE_BUFFER_SIZE];
	char request[WRITE_BUFFER_SIZE]; //parsed request
	bzero(buffer,WRITE_BUFFER_SIZE);
	bzero(request, WRITE_BUFFER_SIZE);
	
	int i = 0, j = 0, n;
	while(1){
		i = i%WRITE_BUFFER_SIZE;

		if(i==0){
			bzero(buffer, WRITE_BUFFER_SIZE);
			n = read(fd,buffer,255);
			printf("n: %d \n", n);
			if (n < 0) error("ERROR reading from socket");
			if (n==0) error("Socket closed");
		}
		
		if(buffer[i]=='.') {
			char * response = process_request(request);

			n = write(fd, response, strlen(response));
			if (n < 0) error("ERROR writing to socket");

			free(response);
			bzero(request, WRITE_BUFFER_SIZE);
			j = 0;
		}
		else if(buffer[i]!='\0'){request[j] = buffer[i]; j++;}

		i++;
	}
	return NULL;
}

int main(int argc, char *argv[])
{

	//initializing stdout lock
	pthread_mutex_init(&stdout_lock, NULL);
	
	// create hashtable
	table = create_hashtable(CAPACITY);

	// opening socket
	int newsockfd, portno;
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
	pthread_t p[MAX_SERVER_THREADS];
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
		if(num_connections==MAX_SERVER_THREADS){
			printf("Too many connections, closing socket\n");
			for(int i =0; i<MAX_SERVER_THREADS; i++)
				pthread_join(p[i], NULL);
			close(sockfd);
			printf("Shutting down server\n");
			return 0;
		}
		// try out pthread_tryjoin_np to get return value from thread
		
	}
	
	free_table(table);
	return 0; 
}
