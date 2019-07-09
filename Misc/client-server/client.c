#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "pthread.h"
#include <time.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

char* gen_rdm_bytestream(size_t num_bytes)
{
  char *stream = malloc(num_bytes);
  size_t i;

  for (i = 0; i<num_bytes; i++)
  {
    stream[i] = rand();
  }

  return stream;
}


struct sockaddr_in serv_addr;
int sockfd;

void * thread(void* args){
	
	char * buffer = gen_rdm_bytestream(256);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    printf("message: %s \n", buffer);

    int n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n", buffer);
	free(buffer);
	return NULL;
}

int main(int argc, char *argv[])
{
    int portno;

    struct hostent *server;

    
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

	pthread_t p1, p2;
	pthread_create(&p1, NULL, thread, NULL);
	pthread_create(&p2, NULL, thread, NULL);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
    return 0;
}
