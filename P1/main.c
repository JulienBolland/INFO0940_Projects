/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "oshell.h"

/*
Important:
    1. DO NOT MODIFY the given skeleton. Nevertheless, if it is easier for you,
you can (only) wrap existing code within conditions (if/else, switch).
    2. Since I'm the TA, I can use static allocation. But you are a student, so
you MUST use dynamic allocation for your data structures. It is an Operating
Systems course after all...
*/

int main() {

    char line[MAX_CMD_SIZE];
    char* arguments[MAX_ARGS];

    int copies;
    int parallel = false;

    do{
        printf("OShell> ");
        fgets(line, sizeof(line), stdin);

        if (line[0] == '\n')
            continue;

        parseCmdLine(line, arguments);

        // Add some stuff here ...
        
        int check = checkCmd(arguments);
        if(check == 1){
          printf("The specified command is not handled by OShell. Please refer \
          to the documentation.\n");
          continue;
        }
        else if(check == -1){
          printf("See you soon!\n");
          break;
        }

        // Number of times to execute a specific command
        do {
            printf("\tCopies> ");
            copies = readCharInput() - '0';
        } while (copies <= 0 || copies > 9);

        // For multiple executions
        if (copies > 1){
            printf("\t[S]equential (default) or [P]arallelize> ");
            parallel = (toupper(readCharInput()) == 'P') ? true : false;
        }

        // Add another stuff here ...

    }while(true);
}
