/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

// Where the includes are located
#include "oshell.h"
// Where prototypes are located
#include "sys.h"

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
  // Open /proc/net/dev, which contains the informations we want
  FILE *file = fopen("/proc/net/dev", "r");
  if(!file){
    perror("File opening error");
    return;
  }
  // This will contain each line of the file
  char* line = (char*) malloc(sizeof(char) * MAX_DATA);
  if(!line){
    fprintf(stderr, "Malloc error\n");
    fclose(file);
    return;
  }
  // The string containing the name of the interface. We ensure a big size of
  // this string (50 in this case, totally arbitrary) in order to avoid
  // segmentation fault
  char* interface = (char*) malloc(sizeof(char) * 50);
  if(!interface){
    fprintf(stderr, "Malloc error\n");
    free(line);
    fclose(file);
    return;
  }
  // Initializing the variables that we will use
  unsigned long int pkts_rcvd, pkts_snt, err_rcvd, err_snt, drop_rcvd, drop_snt;
  float rcvd_err_rate, rcvd_drop_rate, snt_err_rate, snt_drop_rate;

  // Browsing the file. We consider that the length of a line is lower than
  // MAX_DATA.
  for(int i = 0; fgets(line, MAX_DATA, file); i++){
      // The two first lines of /proc/net/dev are titles
      if(i == 0 || i == 1){
        continue;
      }
      // Retrieve the values
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
  // We free what we've allocate
  free(interface);
  free(line);
  fclose(file);
  return;
}

/* -----------------------------------------------------------------------------
 * Execute the operation of a 'sys devstats' command.
 *
 * PARAMETERS
 * path      the path of the directory we want to look into
 * active    pointer containing the adress of an integer, corresponding to the
 *           number of active processes
 * suspended - pointer containing the adress of an integer, corresponding to the
 *             number of suspended processes
 * unsupported - pointer containing the adress of an integer, corresponding to the
 *               number of unsupported processes
 * entry     a structure that will contain the info about the current directory
 *
 * RETURN
 * /
 * ---------------------------------------------------------------------------*/
void devstatsCmd(char *path, long int* active, long int* suspended, \
                 long int* unsupported, struct dirent* entry){
  // We open the directory we want to go in
  DIR *dir = opendir(path);
  if(!dir){
    perror("Directory opening error");
    return;
  }
  // Loop over each file/sub-directory the current directory contains
  while((entry = readdir(dir)) != NULL){
    // This type corresponds to a directory, value found in
    // https://github.com/emscripten-core/emscripten/issues/1277
    if(entry->d_type == 4){
      // if the directory checked if the current or precedent directory
      if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
        continue;
      }
      // Length of the future path we are going to browse in
      int length = strlen(path) + strlen(entry->d_name) + 2;
      // We need a temporary string to perform the recursion
      char* tmp_path = malloc(sizeof(char) * length);
      if(!tmp_path){
       fprintf(stderr, "Malloc error\n");
       break;
      }
      // Concatenation
      snprintf(tmp_path, length, "%s/%s", path, entry->d_name);
      devstatsCmd(tmp_path, active, suspended, unsupported, entry);
      free(tmp_path);
    }
    // If the entry is a file
    else{
      // The information we want to reach is located in "runtime_status" file
      if(!strcmp(entry->d_name, "runtime_status")){
        // cf. computeStates for more information
        if(computeStates(path, entry->d_name, active, suspended, unsupported)\
           != 0){
          break;
        }
      }
    }
  }
  closedir(dir);
  return;
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
  // Length of the total path. We add 3 for the 3 '\0' of each string
  int total_length = strlen("/proc/")+strlen(pid)+strlen("/stat") + 3;
  // The filename we want to take the informations from
  char* filename = malloc(sizeof(char) * total_length);
  if(!filename){
    fprintf(stderr, "Malloc error\n");
    return;
  }
  // Concatenation for the total path of the file
  snprintf(filename, total_length, "/%s/%s/%s", "proc", pid, "stat");
  // Open the file in read mode
  FILE *file = fopen(filename, "r");
  if(!file){
   perror("File opening error");
   goto ALL3;
  }
  // This will contain each line of the file
  char* line = (char*) malloc(sizeof(char) * MAX_DATA);
  if(!line){
   fprintf(stderr, "Malloc error\n");
   goto ALL2;
  }
  // This will contain the name of the process, which is a string
  char* name = (char*) malloc(sizeof(char) * 20);
  if(!name){
    fprintf(stderr, "Malloc error\n");
    goto ALL1;
  }
  // In 'proc/<PID>/stat', the state of a process is represented by a char
  char state;
  // The number of threads of a process
  long int threads;
  // Browsing the file. We consider that the length of a line is lower than
  // MAX_DATA.
  fgets(line, MAX_DATA, file);
  // Place and format of each data for 'proc/<PID>/stat' taken from man page of
  // proc on https://linux.die.net/man/5/proc
  sscanf(line, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d \
                %*d %*d %*d %ld", name, &state, &threads);
  // In this file, the process name is written with parenthesis. This is why we
  // need a temporary string to remove them.
  char* tmp = (char*) malloc(sizeof(char) * 50);
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
  // Print the output
  printf("Process name: %s\n", name);
  printf("Process state: %c ", state);
  // These possible states are some of the stats taken from
  // http://man7.org/linux/man-pages/man5/proc.5.html
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
  // We free everything we have allocated
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
 * Read the file given by path and d_name to know thr process power state.
 *
 * PARAMETERS
 * path      the path of the directory we want to look into
 * d_name    the name of the file
 * active    pointer containing the adress of an integer, corresponding to the
 *           number of active processes
 * suspended - pointer containing the adress of an integer, corresponding to the
 *             number of suspended processes
 * unsupported - pointer containing the adress of an integer, corresponding to the
 *               number of unsupported processes
 *
 * RETURN
 * int      -1 if an error occured, 0 otherwise.
 * ---------------------------------------------------------------------------*/
int computeStates(char* path, char* d_name, long int* active, \
                  long int* suspended, long int* unsupported){
  // Length of the total path of the file
  int length = strlen(path) + strlen(d_name) + 2;
  char* filename = malloc(sizeof(char) * length);
  if(!filename){
   fprintf(stderr, "Malloc error\n");
   return -1;
  }
  // Concatenate the paths into the filename
  snprintf(filename, length, "%s/%s", path, d_name);
  // Open the file
  FILE *file = fopen(filename, "r");
  if(!file){
   perror("File opening error");
   free(filename);
   return -1;
  }
  // This will contain each line of the file
  char* line = (char*) malloc(sizeof(char) * MAX_DATA);
  if(!line){
   fprintf(stderr, "Malloc error\n");
   free(filename);
   fclose(file);
   return -1;
  }
  // Scan the file, which contains only 1 line
  fgets(line, MAX_DATA, file);
  // Gives the string we want to retrieve to 'line'
  sscanf(line, "%s", line);
  // Compare with the possible states  and increment if it fits
  if(strcmp(line, "active") == 0){ *(active) += 1;}
  else if(strcmp(line, "suspended") == 0){ *(suspended) += 1; }
  else if(strcmp(line, "unsupported") == 0){ *(unsupported) += 1; }
  else{ printf("Unknown power state %s\n", line); }
  // Free everything we have allocated
  free(filename);
  free(line);
  fclose(file);
  return 0;
}
