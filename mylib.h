#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PIPE_PATH "/tmp/ipcPipe"
#define handle_error(msg) do { fprintf(stderr, "Errore: %s\n", msg);  exit(EXIT_FAILURE); } while (0)
#define handle_errno_error(msg) do { red(); printf("errno = %d\n", errno); perror(msg); reset(); exit(EXIT_FAILURE); } while (0)


//FUNZIONE CALCOLO PRIMALITA'
int primi(unsigned int a);

//FUNZIONE DI ORDINAMENTO
int compara(const void *a, const void *b); //Serve per far funzionare qsort() -- quicksort presente nelle librerie

//FUNZIONI COLORI SHELL
void red (void);
void green (void);
void blu (void);
void reset (void);