/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#ifndef oshell_h
#define oshell_h

// You can add new header(s), prototype(s) and constant(s)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
  char* cmd;
  unsigned int pid;
  int exit_status;
}metadata;

static const int MAX_CMD_SIZE = 256;                // DO NOT MODIFY
static const int MAX_ARGS = 256;                    // DO NOT MODIFY
static const int MAX_DATA = 256;

char readCharInput(void);                           // DO NOT MODIFY
void parseCmdLine(char* line, char** arguments);    // DO NOT MODIFY

int checkCmd(char** arguments);
metadata* executeCmd(char** arguments, int copies, int parallel);
metadata cd(char** arguments);
metadata showlist();
metadata loadmem();
metadata memdump();


#endif /* oshell_h */
