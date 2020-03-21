/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#include "oshell.h"

// This variable is set global because we are not able to retrieve it in the
// alarm handler otherwise.
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
    // Check if additionnal arguments have been written
    if(arguments[1] != NULL){
      printf("Usage error: Too many arguments.\n");
      return;
    }
    loadmemCmd(meta, nbOfCmd);
  }
  // memdump
  else if(!strcmp(arguments[0], "memdump")){
    // Check if additionnal arguments have been written
    if(arguments[1] != NULL){
      printf("Usage error: Too many arguments.\n");
      return;
    }
    memdumpCmd(meta, *(nbOfCmd));
    for(int i = 0; i < *(nbOfCmd); i++){
      free(meta[i].cmd);
    }
    *(nbOfCmd) = 0;
  }
  // showlist
  else if(!strcmp(arguments[0], "showlist")){
    // Check if additionnal arguments have been written
    if(arguments[1] != NULL){
      printf("Usage error: Too many arguments.\n");
      return;
    }
    showlistCmd(meta, nbOfCmd);
  }
  // For any command in bash
  else{
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        // Execution of the command
        otherCmd(arguments, &(meta[*(nbOfCmd) + i]));
      }
    }
    // Parallel execution
    else{
      // Create a temporary array of 'metadata' to copy in the 'meta' array
      // after the parallel execution
      metadata* tmp = parallelExecution(arguments, copies);
      if(tmp == NULL){
        free(tmp);
        return;
      }
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
void showlistCmd(metadata* meta, int* nbOfCmd){
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
 * meta         an array of a metadata structure
 * nbOfCmd      a pointer on a int containing the number of commands that have
 *              been stored so far
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void loadmemCmd(metadata* meta, int* nbOfCmd){
  // Open the .bin file in read only
  int file_d = open("memdump.bin", O_RDONLY);
  // If an error occured during file openning
  if(file_d < 0){
    perror("File oppening error");
    return;
  }
  // Initializing the values returned by the reads
  int sz_size, sz_cmd, sz_pid, sz_exit, iter = 0;
  // We stop if we reach the maximum data we can make
  for(int i = 0; i < MAX_DATA - *(nbOfCmd); i++){
    // This variable allows us to catch the length of the command string
    size_t current_size;
    sz_size = read(file_d, &(current_size), sizeof(size_t));
    // We allocate memory for the next
    meta[*(nbOfCmd) + i].cmd = malloc(sizeof(char) * (current_size));
    // If malloc failed
    if(meta[*(nbOfCmd) + i].cmd == NULL){
      perror("Malloc error");
      for(int j = 0; j < i; j++){
        free(meta[*(nbOfCmd) + j].cmd);
      }
      if(*(nbOfCmd) > 0){ *(nbOfCmd) -= 1; }
      close(file_d);
      return;
    }
    // str is a temporary string which will contain the name of the command
    char* str = malloc(sizeof(char) * (current_size));
    // If malloc failed
    if(meta[*(nbOfCmd) + i].cmd == NULL){
      perror("Malloc error");
      for(int j = 0; j <= i; j++){
        free(meta[*(nbOfCmd) + j].cmd);
      }
      if(*(nbOfCmd) > 0){ *(nbOfCmd) -= 1; }
      close(file_d);
      return;
    }
    sz_cmd = read(file_d, str, current_size);
    // If we reach EOF, we de-allocate what we've just allocated
    if(sz_size == 0){
      free(meta[*(nbOfCmd) + i].cmd);
      free(str);
      break;
    }
    strcpy(meta[*(nbOfCmd) + i].cmd, str);
    free(str);
    sz_pid = read(file_d, &(meta[*(nbOfCmd) + i].pid), sizeof(pid_t));
    sz_exit = read(file_d, &(meta[*(nbOfCmd) + i].exit_status), sizeof(int));
    // If one of the read failed
    if(sz_size < 0 || sz_cmd < 0 || sz_pid < 0 || sz_exit < 0){
      perror("Reading error");
      for(int j = 0; j <= i; j++){
        free(meta[*(nbOfCmd) + j].cmd);
      }
      close(file_d);
      return;
    }
    iter += 1;
  }
  // Incrementing with the number of iteration, i.e. the number of command added
  *(nbOfCmd) += iter;
  close(file_d);
  return;
}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'memdump' command. This will write in a .bin file
 * the information of a metadata in the following way : size,cmd,pid,exit_status
 * where 'size' is the length of 'cmd'.
 *
 * PARAMETERS
 * meta         an array of a metadata structure
 * nbOfCmd      the number of commands that have been stored so far
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void memdumpCmd(metadata* meta, int nbOfCmd){
  // Open the .bin file : create it if doesn't exist and truncate its content
  // if it exists ; in write only.
  int file_d = open("memdump.bin", O_CREAT | O_WRONLY | O_TRUNC, 0640);
  // If openning file failed
  if(file_d < 0){
    perror("File oppening error");
    return;
  }
  for(int i = 0; i < nbOfCmd; i++){
    // The size of the command we are about to add in the file
    size_t size = strlen(meta[i].cmd)+1;
    int sz_sz = write(file_d, &(size), sizeof(size_t));
    // The command name
    int sz_cmd = write(file_d, meta[i].cmd, size);
    // The pid
    int sz_pid = write(file_d, &(meta[i].pid), sizeof(pid_t));
    // The exit status
    int sz_exit = write(file_d, &(meta[i].exit_status), sizeof(int));
    // If one of the write went wrong
    if(sz_sz < 0 || sz_cmd < 0 || sz_pid < 0 || sz_exit < 0){
      perror("Writing error");
      close(file_d);
      return;
    }
  }
  close(file_d);
  return;
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
  // Setting the alarm to 5 seconds
  alarm(5);
  signal(SIGALRM, alarmHandler);
  // Creating the subprocess that will execute the command
  if((GLOBAL_PID = fork()) == 0){
    if(execvp(arguments[0], arguments) < 0){
      perror("Error while executing command");
      exit(-1);
    }
  }
  // If fork() failed
  else if(GLOBAL_PID < 0){
    perror("Fork failed");
    return;
  }
  int status;
  // Parent process waits for child completion
  waitpid(GLOBAL_PID, &status, 0);
  // If it exited normally
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
  // Remove the alarm
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
 // Array of pid that will store the pid of all children processes
 pid_t* children = malloc(sizeof(pid_t) * copies);
 // The metadata that will be returned
 metadata* meta = malloc(sizeof(metadata) * copies);
 // Checking failure scenarios
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
 // We create as many processes as needed
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
   // Waiting for all children to finish
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
  // Kill the children having the GLOBAL_PID
  kill(GLOBAL_PID, SIGTERM);
}