#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int a = 100;
	int id = fork();
	if (id<0){
		fprintf(stderr, "fork failed \n");
		exit(1);
	}
	else if (id==0) {
		a = 200;
		printf("Child process value: %d \n", a);
		//close(STDOUT_FILENO);
		//open("./p4.output", O_WRONLY|O_TRUNC, S_IRWXU);
		printf("Child process IO");
		printf ("Child : Child’s PID: %d\n", getpid());
    		printf ("Child : Parent’s PID: %d\n", getppid());
	}
	else {
		//while (a<500){
		//	a++;
		//}
		int wr = wait(NULL);
		printf("wait return value: %d \n", wr);
		printf("Parent process value: %d \n", a);
		printf ("Parent : Parent’s PID: %d\n", getpid());
    		printf ("Parent : Child’s PID: %d\n", id);
		close(STDOUT_FILENO);
		open("./p4.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		printf("Wow this is a really long line of nothing, except Parent IO");
	}
	return 0;
}
