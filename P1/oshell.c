/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#include "oshell.h"

static pid_t GLOBAL_PID = 0;

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
    cdCmd(arguments);
  }
  // loadmem
  else if(!strcmp(arguments[0], "loadmem")){
    loadmemCmd(arguments);
  }
  // memdump
  else if(!strcmp(arguments[0], "memdump")){
    memdumpCmd(arguments);
  }
  // showlist
  else if(!strcmp(arguments[0], "showlist")){
    showlistCmd(arguments, meta, nbOfCmd);
  }
  // For any command in bash
  else{
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        otherCmd(arguments, &(meta[*(nbOfCmd) + i]));
      }
    }
    // Parallel execution
    else{
      // Create a temporary array of 'metadata' to copy in the 'meta' array
      // after the parallel execution
      metadata* tmp = parallelExecution(arguments, copies);
      for(int i = 0; i < copies; i++){
        meta[*(nbOfCmd) + i] = tmp[i];
      }
      free(tmp);
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
      printf("(%s;%ld;%d)->", meta[i].cmd, (long)meta[i].pid, \
                                meta[i].exit_status);
    }
    else{
      printf("(%s;%ld;%d)\n", meta[i].cmd, (long)meta[i].pid, \
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

/* -----------------------------------------------------------------------------
 * Execute the specified command once.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 *
 * RETURN
 * metadata     an pointer to a metadata structure, corresponding to the
 *              commands executed
 * ---------------------------------------------------------------------------*/
void otherCmd(char** arguments, metadata* meta){
  if(meta == NULL){
    perror("Malloc error");
    return;
  }
  alarm(5);
  signal(SIGALRM, alarmHandler);
  if((GLOBAL_PID = fork()) == 0){
    if(execvp(arguments[0], arguments) < 0){
      perror("Error while executing command");
      exit(-1);
    }
  }
  else if(GLOBAL_PID < 0){
    perror("Fork failed");
    return;
  }
  int status;
  waitpid(GLOBAL_PID, &status, 0);
  if(WIFEXITED(status)){
    meta->cmd = malloc(sizeof(char) * (1 + strlen(arguments[0])));
    if(meta->cmd == NULL){
      perror("Malloc error");
      return;
    }
    strcpy(meta->cmd, arguments[0]);
    meta->pid = GLOBAL_PID;
    meta->exit_status = WEXITSTATUS(status);
  }
  alarm(0);
  return;
}

/* -----------------------------------------------------------------------------
 * Execute the specified command in a parallel way.
 *
 * PARAMETERS
 * arguments    represents an array of string which contains the command ([0])
 *              and its arguments ([1], [2], ... [255]).
 * copies       specifies the number of times the execution has to be made
 *
 * RETURN
 * metadata     an array of metadata, corresponding to the commands executed by
 *              each parallel processes
 * ---------------------------------------------------------------------------*/
metadata* parallelExecution(char** arguments, int copies){
 pid_t current_pid;
 pid_t* children = malloc(sizeof(pid_t) * copies);
 metadata* meta = malloc(sizeof(metadata) * copies);
 if(children == NULL && meta == NULL){
   perror("Malloc error");
   return NULL;
 }
 else if(children == NULL && meta != NULL){
   free(meta);
   perror("Malloc error");
   return NULL;
 }
 else if(children != NULL && meta == NULL){
   free(children);
   perror("Malloc error");
   return NULL;
 }
 int status;
 for(int i = 0; i < copies; i++){
   if((current_pid = fork()) < 0){
     perror("Fork error");
     free(children);
     free(meta);
     return NULL;
   }
   if(current_pid == 0){
     if(execvp(arguments[0], arguments) < 0){
       exit(-1);
     }
   }
   children[i] = current_pid;
 }
 for(int i = 0; i < copies; i++){
   waitpid(children[i], &status, 0);
   if(WIFEXITED(status)){
       meta[i].cmd = malloc(sizeof(char) * (1 + strlen(arguments[0])));
       if(meta[i].cmd == NULL){
         for(int j = 0; j < i; j++){
           free(meta[j].cmd);
         }
         free(meta);
         free(children);
         perror("Malloc error");
         return NULL;
       }
       strcpy(meta[i].cmd, arguments[0]);
       meta[i].pid = children[i];
       meta[i].exit_status = WEXITSTATUS(status);
   }
 }
 free(children);
 return (meta);
}

/* -----------------------------------------------------------------------------
 * Triggered when a alarm sinal is sent. It kills the problematic process.
 *
 * PARAMETERS
 * sig_num    signal number
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void alarmHandler(int sig_num){
  if(sig_num){
    printf("Process took more than 5 seconds, abort...\n");
  }
  kill(GLOBAL_PID, SIGTERM);
}
