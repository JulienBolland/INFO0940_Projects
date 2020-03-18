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
 * meta         an array of a metadata structure
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
  else if(!strcmp(arguments[0], "showlist")){
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        showlistCmd(arguments, meta, nbOfCmd);
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
        if(pid == 0){
          execvp(arguments[0], arguments);
        }
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            meta[*(nbOfCmd) + i].cmd = malloc(sizeof(char) * \
                                       (1 + strlen(arguments[0])));
            if(meta[*(nbOfCmd) + i].cmd == NULL){
              perror("Malloc error");
              return;
            }
            strcpy(meta[*(nbOfCmd) + i].cmd, arguments[0]);
            meta[*(nbOfCmd) + i].pid = pid;
            meta[*(nbOfCmd) + i].exit_status = WEXITSTATUS(status);
        }
      }
    }
    // Parallel execution
    else{

    }
    // Incrementing the number of commands executed
    *(nbOfCmd) += copies;
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
    chdir("/home");
    return;
  }
  else if(arguments[2] != NULL){
    printf("Usage error: Too many arguments.\n");
    return;
  }
  // We change the directory with the specified path
  chdir(arguments[1]);
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
 * meta         an array of a metadata structure
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void showlistCmd(char** arguments, metadata* meta, int* nbOfCmd){
  if(arguments[1] != NULL){
    printf("Usage error: Too many arguments.\n");
    return;
  }
  for(int i = 0; i < *(nbOfCmd); i++){
    if(i != *(nbOfCmd)-1){
      printf("(%s,%ld,%d)->", meta[i].cmd, (long)meta[i].pid, \
                                meta[i].exit_status);
    }
    else{
      printf("(%s,%ld,%d)\n", meta[i].cmd, (long)meta[i].pid, \
                                meta[i].exit_status);
    }
  }
  return;
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
  char* c = arguments[0];
  for(unsigned int i = 0; i < strlen(c); i++){
    break;
  }
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
  char* c = arguments[0];
  for(unsigned int i = 0; i < strlen(c); i++){
    break;
  }
}
