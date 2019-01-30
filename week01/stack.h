
#ifndef IU_S19_NETWORKS_LABS_STACK_H
#define IU_S19_NETWORKS_LABS_STACK_H

#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE 64

#define COMMAND_PEEK "peek"
#define COMMAND_PUSH "push"
#define COMMAND_POP "pop"
#define COMMAND_EMPTY "empty"
#define COMMAND_DISPLAY "display"
#define COMMAND_CREATE "create"
#define COMMAND_STACK_SIZE "stack_size"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int pop();

int peek();

#endif //IU_S19_NETWORKS_LABS_STACK_H
