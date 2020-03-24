/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#include "oshell.h"

// This variable is set global because we are not able to retrieve it in the
// alarm handler otherwise.
static pid_t GLOBAL_PID = 0;
static metadata* meta = NULL;
static int nbOfCmd = 0;

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
void executeCmd(char** arguments, int copies, int parallel){
  if(meta == NULL){
    meta = malloc(sizeof(metadata) * MAX_DATA);
    if(!meta){
      fprintf(stderr, "Malloc error\n");
      return;
    }
  }
  // exit
  if(isBuiltIn(arguments[0], &(_EXIT))){
    printf("\tSee you soon!\n");
    for(int j = 0; j < nbOfCmd; j++){
      free(meta[j].cmd);
    }
    free(meta);
    exit(0);
  }
  // cd
  if(isBuiltIn(arguments[0], &(_CD))){
    cdCmd(arguments);
  }
  // loadmem
  else if(isBuiltIn(arguments[0], &(_LOADMEM))){
    // Check if additionnal arguments have been written
    if(arguments[1] != NULL){
      fprintf(stderr, "LOADMEM error: Too many arguments.\n");
      return;
    }
    loadmemCmd();
  }
  // memdump
  else if(isBuiltIn(arguments[0], &(_MEMDUMP))){
    // Check if additionnal arguments have been written
    if(arguments[1] != NULL){
      fprintf(stderr, "MEMDUMP error: Too many arguments.\n");
      return;
    }
    memdumpCmd();
    for(int i = 0; i < nbOfCmd; i++){
      free(meta[i].cmd);
    }
    nbOfCmd = 0;
  }
  // showlist
  else if(isBuiltIn(arguments[0], &(_SHOWLIST))){
    // Check if additionnal arguments have been written
    if(arguments[1] != NULL){
      fprintf(stderr, "SHOWLIST error: Too many arguments.\n");
      return;
    }
    showlistCmd();
  }
  // sys
  else if(isBuiltIn(arguments[0], &(_SYS))){
    if(arguments[1] == NULL){
      fprintf(stderr, "SYS error: Missing argument(s).\n");
      return;
    }
    if(isBuiltIn(arguments[1], &(_NETSTATS))){
      netstatsCmd();
    }
    else if(isBuiltIn(arguments[1], &(_DEVSTATS))){
      devstatsCmd();
    }
    else if(isBuiltIn(arguments[1], &(_STATS))){
      if(arguments[2] == NULL){
        fprintf(stderr, "SYS STATS error: Missing argument.\n");
        return;
      }
      statsCmd(arguments[2]);
    }
    else{
      fprintf(stderr, "SYS error: Wrong argument.\n");
      return;
    }
  }
  // For any command in bash
  else{
    // Sequential execution
    if(!parallel){
      for(int i = 0; i < copies; i++){
        // Execution of the command
        otherCmd(arguments);
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
        meta[nbOfCmd + i] = tmp[i];
      }
      free(tmp);
      // Incrementing the number of commands executed
      nbOfCmd += copies;
    }
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
    chdir(getenv("HOME"));
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
void showlistCmd(void){
  for(int i = 0; i < nbOfCmd; i++){
    if(i != nbOfCmd-1){
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
void loadmemCmd(void){
  // Open the .bin file in read only
  int file_d = open("memdump.bin", O_RDONLY, 0644);
  // If an error occured during file openning
  if(file_d < 0){
    perror("File opening error");
    return;
  }
  // Initializing the values returned by the reads
  int sz_size, sz_cmd, sz_pid, sz_exit, iter = 0;
  // We stop if we reach the maximum data we can make
  for(int i = 0; i < MAX_DATA - nbOfCmd; i++){
    // This variable allows us to catch the length of the command string
    size_t current_size;
    sz_size = read(file_d, &(current_size), sizeof(size_t));
    // We allocate memory for the next
    meta[nbOfCmd + i].cmd = malloc(sizeof(char) * (current_size));
    // If malloc failed
    if(meta[nbOfCmd + i].cmd == NULL){
      perror("Malloc error");
      for(int j = 0; j < i; j++){
        free(meta[nbOfCmd + j].cmd);
      }
      if(nbOfCmd > 0){ nbOfCmd -= 1; }
      close(file_d);
      return;
    }
    // str is a temporary string which will contain the name of the command
    char* str = malloc(sizeof(char) * (current_size));
    // If malloc failed
    if(meta[nbOfCmd + i].cmd == NULL){
      perror("Malloc error");
      for(int j = 0; j <= i; j++){
        free(meta[nbOfCmd + j].cmd);
      }
      if(nbOfCmd > 0){ nbOfCmd -= 1; }
      close(file_d);
      return;
    }
    sz_cmd = read(file_d, str, current_size);
    // If we reach EOF, we de-allocate what we've just allocated
    if(sz_size == 0){
      free(meta[nbOfCmd + i].cmd);
      free(str);
      break;
    }
    strcpy(meta[nbOfCmd + i].cmd, str);
    free(str);
    sz_pid = read(file_d, &(meta[nbOfCmd + i].pid), sizeof(pid_t));
    sz_exit = read(file_d, &(meta[nbOfCmd + i].exit_status), sizeof(int));
    // If one of the read failed
    if(sz_size < 0 || sz_cmd < 0 || sz_pid < 0 || sz_exit < 0){
      perror("Reading error");
      for(int j = 0; j <= i; j++){
        free(meta[nbOfCmd + j].cmd);
      }
      close(file_d);
      return;
    }
    iter += 1;
  }
  // Incrementing with the number of iteration, i.e. the number of command added
  nbOfCmd += iter;
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
void memdumpCmd(void){
  // Open the .bin file : create it if doesn't exist and truncate its content
  // if it exists ; in write only.
  int file_d = open("memdump.bin", O_CREAT | O_WRONLY | O_TRUNC, 0644);
  // If openning file failed
  if(file_d < 0){
    perror("File opening error");
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
 * Execute the operation of a 'sys netstats' command.
 *
 * PARAMETERS
 * /
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void netstatsCmd(void){
  FILE *file = fopen("/proc/net/dev", "r");
  if(!file){
    perror("File opening error");
    return;
  }
  char* line = (char*) malloc(sizeof(char) * MAX_DATA);
  if(!line){
    fprintf(stderr, "Malloc error\n");
    fclose(file);
    return;
  }
  char* interface = (char*) malloc(sizeof(char) * 10);
  if(!interface){
    fprintf(stderr, "Malloc error\n");
    free(line);
    fclose(file);
    return;
  }
  unsigned long int pkts_rcvd, pkts_snt, err_rcvd, err_snt, drop_rcvd, drop_snt;
  float rcvd_err_rate, rcvd_drop_rate, snt_err_rate, snt_drop_rate;

  for(int i = 0; fgets(line, MAX_DATA, file); i++){
      // The two first lines of /proc/net/dev are titles
      if(i == 0 || i == 1){
        continue;
      }
      sscanf(line, "%s %*u %lu %lu %lu %*u %*u %*u %*u %*u %lu %lu %lu ", \
             interface, &pkts_rcvd, &err_rcvd, &drop_rcvd,  &pkts_snt, \
             &err_snt, &drop_snt);

      // Checking to avoid division by 0
      if(pkts_rcvd == 0){ rcvd_err_rate = 0; }
      else{ rcvd_err_rate = err_rcvd/pkts_rcvd; }

      if(pkts_snt == 0){ snt_err_rate = 0; }
      else{ snt_err_rate = err_snt/pkts_snt; }

      if(pkts_rcvd == 0){ rcvd_drop_rate = 0; }
      else{ rcvd_drop_rate = drop_rcvd/pkts_rcvd; }

      if(pkts_snt == 0){ snt_drop_rate = 0; }
      else{ snt_drop_rate = drop_snt/pkts_snt; }

      // Inconsistency writting due to bad output format
      printf("%s Rx(pkts : %lu, err : %f%%, drop : %f%%) - Tx(pkts : %lu,\
 err: %f%%, drop : %f%%)\n", interface, pkts_rcvd, rcvd_err_rate, \
             rcvd_drop_rate, pkts_snt, snt_err_rate, snt_drop_rate);
  }
  free(interface);
  free(line);
  fclose(file);
  return;
}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'sys devstats' command.
 *
 * PARAMETERS
 * /
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void devstatsCmd(void){
  printf("In devstats.\n" );
}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'sys stats <PID>' command.
 *
 * PARAMETERS
 * pid       the pid of the process we want the stats on
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void statsCmd(char* pid){
  int total_length = strlen("/proc/")+strlen(pid)+strlen("/status") + 3;
  char* filename = malloc(sizeof(char) * total_length);
  if(!filename){
    perror("Malloc error");
    return;
  }
  snprintf(filename, total_length, "%s%s%s", "/proc/", pid, "/stat");

  FILE *file = fopen(filename, "r");
  if(!file){
   perror("File opening error");
   goto ALL3;
  }
  char* line = (char*) malloc(sizeof(char) * MAX_DATA);
  if(!line){
   fprintf(stderr, "Malloc error\n");
   goto ALL2;
  }
  char* name = (char*) malloc(sizeof(char) * 20);
  if(!name){
    fprintf(stderr, "Malloc error\n");
    goto ALL1;
  }
  char state;
  long int threads;
  fgets(line, MAX_DATA, file);
  // Place and format of each data for 'proc/<PID>/stat' taken from man page of
  // proc on https://linux.die.net/man/5/proc
  sscanf(line, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d \
                %*d %*d %*d %ld", name, &state, &threads);
  char* tmp = (char*) malloc(sizeof(char) * 20);
  if(!tmp){
    fprintf(stderr, "Malloc error\n");
    goto ALL;
  }
  int j = 0;
  for(int i = 0; name[i] != '\0'; i++){
    if(name[i] == '(' || name[i] == ')'){ continue; }
    tmp[j++] = name[i];
  }
  tmp[j] = '\0';
  strcpy(name, tmp);
  free(tmp);
  printf("Process name: %s\n", name);
  printf("Process state: %c ", state);
  switch(state){
    case 'S': printf("(sleeping)\n"); break;
    case 'R': printf("(running)\n"); break;
    case 'D': printf("(disk sleep)\n"); break;
    case 'Z': printf("(zombie)\n"); break;
    case 'T': printf("(traced or stopped)\n"); break;
    case 'W': printf("(paging)\n"); break;
    case 'I': printf("(idle)\n"); break;
    default: printf("\n"); break;
  }
  printf("Process threads: %ld\n", threads);

  ALL:
  free(name);
  ALL1:
  free(line);
  ALL2:
  fclose(file);
  ALL3:
  free(filename);
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
void otherCmd(char** arguments){
  // Setting the alarm to 5 seconds
  alarm(5);
  signal(SIGALRM, signalHandler);
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
    meta[nbOfCmd].cmd = malloc(sizeof(char) * (1 + strlen(arguments[0])));
    if(meta[nbOfCmd].cmd == NULL){
      perror("Malloc error");
      return;
    }
    strcpy(meta[nbOfCmd].cmd, arguments[0]);
    meta[nbOfCmd].pid = GLOBAL_PID;
    meta[nbOfCmd].exit_status = WEXITSTATUS(status);
  }
  // Remove the alarm
  alarm(0);
  nbOfCmd++;
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
void signalHandler(int sig_num){
  if(sig_num == SIGALRM){
    fprintf(stderr, "Process took more than 5 seconds to complete, abort...\n");
    // Kill the children having the GLOBAL_PID
    kill(GLOBAL_PID, SIGTERM);
  }
  else if(sig_num == SIGINT){
    printf("\n\tSee you soon!\n");
    exit(0);
  }
}

/* -----------------------------------------------------------------------------
 * Checks if the specified command is a built-in command. Allow to compare
 * directly with a specific built-in commands.
 *
 * PARAMETERS
 * cmd        the command specified by the user
 * loc        the identifier of the built-in command we want to compare cmd to
 *
 * RETURN
 * bool       true if the command is a built-in one, false otherwise.
 * ---------------------------------------------------------------------------*/
bool isBuiltIn(char* arg, const int* loc){
  // The number of built-in commands
  int nb = 9;
  char** built_in = (char**) malloc(sizeof(char*) * nb);
  if(!built_in){
    perror("Malloc error");
    return false;
  }
  for(int i = 0; i < nb; i++){
    built_in[i] = (char*) malloc(sizeof(char) * 30);
    if(!built_in[i]){
      for(int j = 0; j < i; j++){ free(built_in[j]); }
      free(built_in);
      perror("Malloc error");
      return false;
    }
  }
  strcpy(built_in[_CD], "cd");
  strcpy(built_in[_LOADMEM], "loadmem");
  strcpy(built_in[_MEMDUMP], "memdump");
  strcpy(built_in[_SHOWLIST], "showlist");
  strcpy(built_in[_SYS], "sys");
  strcpy(built_in[_EXIT], "exit");
  strcpy(built_in[_NETSTATS], "netstats");
  strcpy(built_in[_DEVSTATS], "devstats");
  strcpy(built_in[_STATS], "stats");
  if(!loc){
    for(int i = 0; i < nb; i++){
      if(!strcmp(arg, built_in[i])){
        for(int j = 0; j < nb; j++){ free(built_in[j]); }
        free(built_in);
        return true;
      }
    }
  }
  else{
    if(!strcmp(arg, built_in[*(loc)])){
      for(int j = 0; j < nb; j++){
        free(built_in[j]);
      }
      free(built_in);
      return true;
    }
  }
  for(int j = 0; j < nb; j++){ free(built_in[j]); }
  free(built_in);
  return false;
}
