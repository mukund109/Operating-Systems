#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

void leak(){
	int * mem = malloc(sizeof(int)*100);
	//mem[100] = 234;
	free(mem);
	mem[4]=23;
}
int main(int argc, char *argv[]){
		//int * ptr = NULL;
		//int num = *ptr;
		//printf("%d \n", num);

		leak();
		return 0;
}