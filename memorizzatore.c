#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "mylib.h"

// Compilare con -DDEBUG se si vogliono avere a video più informazioni durante l'intero processo

#ifndef DEBUG
    #define DEBUG 0
#else
    #define DEBUG 1
#endif 

int main(int argc, char  *argv[]) 
{
    int fd;
    int *array, *new, len_array_tot = 2, len_array_pieno;
    int letto;
    FILE* f;
    char SAVE_PATH[128];


    if((fd = open(PIPE_PATH, O_RDONLY)) == -1) 
        handle_errno_error("Errore apertura fd pipe in memorizzatore");
    else
        printf("\nRicevimento dati dai calcolatori in corso...\n");

    if(DEBUG) printf("\n");

    if ((array = (int*) calloc(len_array_tot, sizeof(int))) == NULL)
        handle_error("calloc -- memoria non allocata in memorizzatore");

    //Inizio lettura da pipe
    for(int i = 0 ; ; i++) {

        if((i + 1) > len_array_tot) { // Controllo se sta per finire la capienza del mio array
            if((new = (int*) realloc(array, 2 * len_array_tot * sizeof(int))) == NULL) // Raddoppio grandezza array dinamico
                handle_error("realloc -- memoria non allocata in memorizzatore");
            else {
                array = new;
                len_array_tot = len_array_tot * 2;
            }   
        }

        if((letto = read(fd, &array[i], sizeof(int))) == -1)
            handle_errno_error("Errore lettura pipe");
        else if (letto == 0){
            if (close(fd) == -1) 
                handle_errno_error("Errore chiusura pipe nel memorizzatore");
            else
                printf("\nChiusura pipe nel memorizzatore...\n\n");
            printf("Ho finito di leggere... valori totali letti: "); green(); printf("%d\n\n", i); reset();
            len_array_pieno = i;
            break;
        }

        if(DEBUG) { printf("Memorizzatore: letto "); green(); printf("%d\n", array[i]); reset(); }
    }

    unlink(PIPE_PATH);


    printf("Inizio ordinamento valori primi trovati...\n");
    qsort(array, len_array_pieno, sizeof(int), compara);
    printf("Ordinamento valori primi terminato\n\n");

    strcpy(SAVE_PATH, argv[1]);
    if(DEBUG) printf("SAVE_PATH = %s\n", SAVE_PATH);

    if((f = fopen(SAVE_PATH, "w")) == NULL)
        handle_errno_error("Errore creazione file numeri primi");
    else
        printf("Creazione file numeri primi in corso...\n\n");


    for(int i = 0; i < len_array_pieno; i++) {
        if((letto = fprintf(f, "%d\n", array[i])) < 0)
            handle_error("Errore scrittura file numeri primi");
        else
            { printf("Sto salvando: "); green(); printf("%d\n", array[i]); reset(); }
    }

    if (fclose(f) == EOF) 
        handle_errno_error("Errore chiusura file numeri primi");
    else {
        printf("\nChiusura file numeri primi...\n");
        exit(0);
    }
    
    
    return EXIT_SUCCESS;
}