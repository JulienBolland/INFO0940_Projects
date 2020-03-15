/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#include "oshell.h"

/* -----------------------------------------------------------------------------
 * Parse a command line into arguments.
 * /!\ DO NOT MODIFY /!\
 *
 * PARAMETERS
 * line         represents the line as a single string (unparsed).
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void parseCmdLine(char* line, char** arguments) {
    int i = 0;

    line[strlen(line) - 1] = '\0';
    arguments[i] = strtok(line, " ");
    while (arguments[i++] && i < MAX_ARGS) {
        arguments[i] = strtok(NULL, " ");
    }
}

/* -----------------------------------------------------------------------------
 * Read a character from the user input and discard up to the newline.
 * /!\ DO NOT MODIFY /!\
 *
 * PARAMETERS
 * /
 *
 * RETURN
 * char     a single character of user input.
 * ---------------------------------------------------------------------------*/
char readCharInput(void) {
    char c = getchar();

    while(getchar() != '\n');
    return c;
}

//-----------------------------------------------------------------------------
// Here you can add your creepy stuff...

/* -----------------------------------------------------------------------------
 * Check whether the command stored in 'arguments' is handled by OShell.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * int     1 if the command is handled, 0 otherwise and -1 if it is the exit
 *         command.
 * ---------------------------------------------------------------------------*/
int checkCmd(char** arguments){
  if(!strcmp(arguments[0], "cd")||!strcmp(arguments[0], "memdump")|| \
     !strcmp(arguments[0], "loadmem")||!strcmp(arguments[0], "showlist")){
       return 1;
  }
  else if(!strcmp(arguments[0], "exit")){
    return -1;
  }
  return 0;
}

/* -----------------------------------------------------------------------------
 * Call the function corresponding to the command given by the user.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 * copies       specifies the number of times the execution has to be made
 * parallel     specifies if the execution is parallel or sequential
 *
 * RETURN
 * metadata     struct defining the commands that have been executed
 * ---------------------------------------------------------------------------*/
metadata* executeCmd(char** arguments, int copies, int parallel){
  metadata* meta = malloc(sizeof(metadata)*10);
  // cd
  if(!strcmp(arguments[0], "cd")){
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        meta[i] = cd(arguments);
      }
    }
    // Parallel execution
    else{

    }
  }
  // loadmem
  else if(!strcmp(arguments[0], "loadmem")){

  }
  // memdump
  else if(!strcmp(arguments[0], "memdump")){

  }
  // showlist
  else{

  }
  return meta;
}
