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
 * Call the function corresponding to the command given by the user.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 * copies       specifies the number of times the execution has to be made
 * parallel     specifies if the execution is parallel or sequential
 * meta         a pointer on a metadata structure
 * nbOfCmd      a pointer on a int containing the number of commands that have
 *              been stored so far
 *
 * RETURN
 * metadata     struct defining the commands that have been executed
 * ---------------------------------------------------------------------------*/
void executeCmd(char** arguments, int copies, int parallel, \
                metadata* meta, int* nbOfCmd){
  // cd
  if(!strcmp(arguments[0], "cd")){
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        cdCmd(arguments);
      }
    }
    // Parallel execution
    else{

    }
  }
  // loadmem
  else if(!strcmp(arguments[0], "loadmem")){
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        loadmemCmd(arguments);
      }
    }
    // Parallel execution
    else{

    }
  }
  // memdump
  else if(!strcmp(arguments[0], "memdump")){
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        memdumpCmd(arguments);
      }
    }
    // Parallel execution
    else{

    }
  }
  // showlist
  else if(!strcmp(arguments[0], "memdump")){
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        showlistCmd(arguments);
      }
    }
    // Parallel execution
    else{

    }
  }
  else{
    pid_t pid;
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        pid = fork();
        if (pid == 0){
          //meta[*(nbOfCmd) + i]->cmd = arguments[0];
          execvp(arguments[0], arguments);
        }
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            meta[*(nbOfCmd) + i].pid = pid;
            meta[*(nbOfCmd) + i].exit_status = WEXITSTATUS(status);
        }
      }
    }
    // Parallel execution
    else{

    }
    // Incrementing the number of commands executed
    nbOfCmd += copies;
  }
}

/* -----------------------------------------------------------------------------
 * Execute the operations of a 'cd' command.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void cdCmd(char** arguments){
  // Reset error number at each time, to avoid conflict with old error
  errno = 0;
  // If no argument or if '~' (ASCII => 126) is given, we go to /home
  if(arguments[1] == NULL || *arguments[1] == 126){
    int exit_status = chdir("/home");
    return;
  }
  // We change the directory with the specified path
  int exit_status = chdir(arguments[1]);
  // If an error occured
  if(errno == ENOENT || errno == ENOTDIR || errno == EACCES || errno == EIO){
    perror("Usage error");
    return;
  }
  return;
}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'showlist' command.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void showlistCmd(char** arguments){

}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'loadmem' command.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void loadmemCmd(char** arguments){

}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'memdump' command.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void memdumpCmd(char** arguments){

}
