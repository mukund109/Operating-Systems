#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int m = atoi(argv[1]);
	
	long num = 1000000*m;
	size_t size = sizeof(char)*num;
	
	char* array = (char*)malloc(size);
	printf("Process ID: %d", getpid());
	for(int i=0; i<num; i++){
		sleep(0.1);
		printf("%c", *(array+i));
	}
	printf("success");
}