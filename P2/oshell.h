/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#ifndef oshell_h
#define oshell_h

// You can add new header(s), prototype(s) and constant(s)

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct{
  char* cmd;
  pid_t pid;
  int exit_status;
}metadata;

static const int MAX_CMD_SIZE = 256;                // DO NOT MODIFY
static const int MAX_ARGS = 256;                    // DO NOT MODIFY
static const int MAX_DATA = 256;

char readCharInput(void);                           // DO NOT MODIFY
void parseCmdLine(char* line, char** arguments);    // DO NOT MODIFY

void executeCmd(char** arguments, int copies, int parallel, \
                metadata* meta, int* nbOfCmd);
void cdCmd(char** arguments);
void showlistCmd(metadata* meta, int* nbOfCmd);
void loadmemCmd(metadata* meta, int* nbOfCmd);
void memdumpCmd( metadata* meta, int nbOfCmd);
void otherCmd(char** arguments, metadata* meta);
metadata* parallelExecution(char** arguments, int copies);
void alarmHandler(int sig_num);


#endif /* oshell_h */
