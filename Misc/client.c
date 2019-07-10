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
#include <assert.h>
#include "buffer.h"

/* protocol: client request
		first character : 's'(set) or 'g'(get) or 'd'(delete)
		followed by integer key
		if 's', its followed by key, further followed by string
		each request ends with '.'

   protocol: server response
		first character : '0', '1', or string with response of get call
		note - this protocol doesn't reliable read all the bytes of the response string, this is because there's no terminating character
		
*/

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void gen_random_command(char * buffer){
	assert(3+NUM_DIGITS_IN_KEY+NUM_CHARS_IN_VALUE <= WRITE_BUFFER_SIZE);
	bzero(buffer, WRITE_BUFFER_SIZE);
	int r = rand()%3;
	int i;
	for(i = 1; i<NUM_DIGITS_IN_KEY+1; i++) 
		buffer[i] = rand()%10 + 48;
	
	if(r == 0){
		buffer[0] = 's';
		for(;i<NUM_DIGITS_IN_KEY+NUM_CHARS_IN_VALUE+1; i++)
			buffer[i] = rand()%2 ? rand()%26+65 : rand()%26+97;
	}
	else if(r==1)
		buffer[0] = 'g';
	else
		buffer[0] = 'd';

	buffer[i] = '.';
}

struct sockaddr_in serv_addr;
int sockfd;

void * thread(void* args){
	char * write_buffer = malloc(WRITE_BUFFER_SIZE);
	char * read_buffer = malloc(READ_BUFFER_SIZE);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
	//TODO: implement time-out
	for(int j=0; j<20; j++){
		gen_random_command(write_buffer);
		printf("message: %s \n", write_buffer);
	
		printf("write buffer strlen=%lu \n", strlen(write_buffer));
		int n = write(sockfd, write_buffer, strlen(write_buffer));
		if (n < 0) 
		     error("ERROR writing to socket");

		bzero(read_buffer, READ_BUFFER_SIZE);
		n = read(sockfd, read_buffer, READ_BUFFER_SIZE-1);
		if (n < 0) 
		     error("ERROR reading from socket");
		printf("%s\n", read_buffer);
		sleep(1);
	}
	printf("ending thread \n");
	free(read_buffer);
	free(write_buffer);
	return NULL;
}

int main(int argc, char *argv[])
{
    int portno;

    struct hostent *server;

    srand((unsigned int)time(NULL));
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
