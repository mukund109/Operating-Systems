#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "pthread.h"
#include <sys/time.h>
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

int total_requests;
struct timeval start_r, end_r;
pthread_mutex_t total_requests_lock;
FILE *logger;

// increments total request counter and writes to log file
void increment_total_requests(int num){
	pthread_mutex_lock(&total_requests_lock);
	total_requests += num;
	if(total_requests>100000){
		gettimeofday(&end_r, NULL); 
		float time_taken = ((end_r.tv_sec*1e6 + end_r.tv_usec) - (start_r.tv_sec*1e6 + start_r.tv_usec))/1e6;

		fprintf(logger, "%.4f %ld \n", total_requests/time_taken,  end_r.tv_sec);
		fflush(logger);
		printf("throughput (total requests served per sec) : %.4f", total_requests/time_taken);
		total_requests = 0;
		gettimeofday(&start_r, NULL);
	}	
	pthread_mutex_unlock(&total_requests_lock);
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
    if(sockfd < 0)
        error("ERROR opening socket");
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){error("ERROR connecting");}
	
	float sum_latency = 0;
	int update_time_period = 3000;

	//TODO: implement time-out
	for(int j=1; j<REQUESTS_PER_THREAD+1; j++){
		gen_random_command(write_buffer);
		//printf("message: %s \n", write_buffer);
	
		//printf("write buffer strlen=%lu \n", strlen(write_buffer));
		struct timeval start, end; 
		gettimeofday(&start, NULL);

		int n = write(sockfd, write_buffer, strlen(write_buffer));
		if (n < 0) 
		     error("ERROR writing to socket");
		bzero(read_buffer, READ_BUFFER_SIZE);
		n = read(sockfd, read_buffer, READ_BUFFER_SIZE-1);
		gettimeofday(&end, NULL); 
		if (n < 0 || n==0) 
		     error("ERROR reading from socket");
		
		float latency = ((end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec))/1e6;
		sum_latency += latency;
		
		if(j%update_time_period == 0){
			printf("%s, latency: %.6f, avg_latency: %.6f, loop num: %d \n", read_buffer, latency, sum_latency/update_time_period, j);
			increment_total_requests(update_time_period);
			sum_latency = 0;
		}

	}
	close(sockfd);
	printf("avg latency: %.6f \n", sum_latency/REQUESTS_PER_THREAD);
	printf("thread terminated\n");
	free(read_buffer);
	free(write_buffer);
	return NULL;
}

int main(int argc, char *argv[])
{
	// initializing total request counter and clock
    pthread_mutex_init(&total_requests_lock, NULL);
	total_requests = 0;

	//opening logging file
	logger = fopen("logs", "w+");

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

	pthread_t p[MAX_SERVER_THREADS];
	gettimeofday(&start_r, NULL);	
	for(int i = 0; i<MAX_SERVER_THREADS; i++){
		pthread_create(&p[i], NULL, thread, NULL);
	}
	for(int i = 0; i<MAX_SERVER_THREADS; i++){
		pthread_join(p[i], NULL);
	}
	
	fclose(logger);
    return 0;
}
