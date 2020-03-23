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
static const int _CD = 0;
static const int _LOADMEM = 1;
static const int _MEMDUMP = 2;
static const int _SHOWLIST = 3;
static const int _EXIT = 4;


char readCharInput(void);                           // DO NOT MODIFY
void parseCmdLine(char* line, char** arguments);    // DO NOT MODIFY

void executeCmd(char** arguments, int copies, int parallel);
void cdCmd(char** arguments);
void showlistCmd(void);
void loadmemCmd(void);
void memdumpCmd(void);
void otherCmd(char** arguments);
metadata* parallelExecution(char** arguments, int copies);
void signalHandler(int sig_num);
bool isBuiltIn(char* arg, const int* loc);


#endif /* oshell_h */
