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
void devstatsCmd(char *path, long int* active, long int* suspended, \
                 long int* unsupported){
  DIR *dir = opendir(path);
  if(!dir){
    perror("Directory opening error");
    return;
  }
  struct dirent *entry = malloc(sizeof(struct dirent));
  if(!entry){
    perror("Malloc error");
    goto CLOSE;
  }

  while((entry = readdir(dir)) != NULL){
    if(entry->d_type == 4){
      if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
        continue;
      }
      int length = strlen(path) + strlen(entry->d_name) + 2;
      char* tmp_path = malloc(sizeof(char) * length);
      if(!tmp_path){
       fprintf(stderr, "Malloc error\n");
       goto FREE;
      }
      snprintf(tmp_path, length, "%s/%s", path, entry->d_name);
      devstatsCmd(tmp_path, active, suspended, unsupported);
      free(tmp_path);
    }
    else{
      if(!strcmp(entry->d_name, "runtime_status")){
        int length = strlen(path) + strlen(entry->d_name) + 2;
        char* filename = malloc(sizeof(char) * length);
        if(!filename){
         fprintf(stderr, "Malloc error\n");
         goto FREE;
        }
        snprintf(filename, length, "%s/%s", path, entry->d_name);
        FILE *file = fopen(filename, "r");
        if(!file){
         perror("File opening error");
         free(filename);
         goto FREE;
        }
        char* line = (char*) malloc(sizeof(char) * MAX_DATA);
        if(!line){
         fprintf(stderr, "Malloc error\n");
         free(filename);
         fclose(file);
         goto FREE;
        }
        fgets(line, MAX_DATA, file);
        sscanf(line, "%s", line);
        if(strcmp(line, "active") == 0){ *(active) += 1;}
        else if(strcmp(line, "suspended") == 0){ *(suspended) += 1; }
        else if(strcmp(line, "unsupported") == 0){ *(unsupported) += 1; }
        else{ printf("Unknown power state\n"); }
        free(filename);
        free(line);
        fclose(file);
      }
    }
  }
  FREE:
  free(entry);
  CLOSE:
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
  int total_length = strlen("/proc/")+strlen(pid)+strlen("/status") + 3;
  char* filename = malloc(sizeof(char) * total_length);
  if(!filename){
    perror("Malloc error");
    return;
  }
  snprintf(filename, total_length, "/%s/%s/%s", "proc", pid, "stat");

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
