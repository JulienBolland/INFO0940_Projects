/*
Group: 26
Member1: s161622 - Bolland - Julien
Member2: s162425 - Gilson - Maxence
*/
#ifndef sys_h
#define sys_h

/* -----------------------------------------------------------------------------
 * Functions needed for the second part of the project.
 * ---------------------------------------------------------------------------*/
 
void netstatsCmd(void);
void devstatsCmd(char *path, long int* active, long int* suspended, \
                 long int* unsupported);
void statsCmd(char* pid);

#endif /* sys_h */
