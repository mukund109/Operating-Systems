#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
  boolean a = 1;
  char *buf = "what";
  write(STDOUT_FILENO, "wait", 1);
  printf("%zu \n", sizeof(double));
  return 0;
}
