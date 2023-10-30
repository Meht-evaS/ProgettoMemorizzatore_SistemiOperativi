#include <pthread.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include "mylib.h"

// Compilare con -DDEBUG se si vogliono avere a video più informazioni durante l'intero processo

#ifndef DEBUG
	#define DEBUG 0
#else
	#define DEBUG 1
#endif 


int n = 0; // n: numero di elementi di cui testare la primalità 
int *p1, *p2, *p3; // Array dinamici dove salvare i numeri interi che trovano i thread
int val_primi_th1 = 0, val_primi_th2 = 0, val_primi_th3 = 0; //Numero totale valori primi trovati da ogni thread 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex a livello globale per accedere a n

// A proprio rischio e pericolo si possono usare le printf con colore che io ho commentato (RIGHE 40 e 79)
// Le ho commentate in quanto non sono threadsafe e creavano un pò di confusione
void calcolatore(int *arg)
{   
	int local_n = 0, result = 0, val_calcolati = 0, val_primi = 0, len_array_tot = 2, *p, *new; 
	printf("Creazione calcolatore n. %d\n", *arg); 

	p = (int*) calloc(len_array_tot, sizeof(int)); // Mi creo, per ogni thread, un array dinamico per i numeri primi trovati

	while(1) {

		pthread_mutex_lock(&mutex); //Sincronizzo l'accesso alla var globale n per leggerla e decrementarla
		if(n > 0) {
			local_n = n;
			//printf("Calcolatore n. %d: inizio calcolo primalità per n = ", *arg); green(); printf("%d\n", n);  reset(); }
			printf("Calcolatore n. %d: inizio calcolo primalità per n = %d\n", *arg, n);
			n--;
		}
		else {
			pthread_mutex_unlock(&mutex);
			red(); printf("Uscita calcolatore n. %d  -- Valori calcolati: %d\n", *arg, val_calcolati); reset();

			switch(*arg) {
			case 1 :
				if(DEBUG) { green(); printf("Calcolatore n. %d -- valori primi trovati da th1 = %d\n", *arg, val_primi); reset(); }
				// Salvo a globale i valori dei singoli thread così da averli anche nel main 
				val_primi_th1 = val_primi;	
				p1 = p;
				break;
			case 2 :
				if(DEBUG) { green(); printf("Calcolatore n. %d -- valori primi trovati da th2 = %d\n", *arg, val_primi); reset(); }
				val_primi_th2 = val_primi;				
				p2 = p;
			 	break;
			case 3 :
				if(DEBUG) { green(); printf("Calcolatore n. %d -- valori primi trovati da th3 = %d\n", *arg, val_primi); reset(); }
			 	val_primi_th3 = val_primi;			 	
				p3 = p;
			 	break;
			default :
			 	printf("switch -- calcolatore n. %d\n", *arg);
			 	handle_error("switch -- corruzione argomento thread");

   			}

			pthread_exit(NULL);

		}			

		pthread_mutex_unlock(&mutex); // Sblocco la mutex così che anche gli altri possono ottenere un nuovo numero
									  // e nel mentre mi calcolo se il numero appena ottenuto è primo 

		if((result = primi((unsigned int) local_n)) != 0) { //return 0 se NON primo, 1 se primo
			
			//if(DEBUG) { printf("n: "); blu(); printf("%d ", local_n); reset(); printf("è un numero primo\n"); }
			if(DEBUG) { printf("n: %d è un numero primo\n", local_n); }
			val_primi++; 
			
			if((val_primi + 1) > len_array_tot) { // Controllo se sta per finire la capienza del mio array
				if((new = (int*) realloc(p, 2 * len_array_tot * sizeof(int))) == NULL) // Raddoppio capienza array dinamico
		    		handle_error("realloc -- memoria non allocata");
				else {
					p = new;
					len_array_tot = len_array_tot * 2;			
				}
			}

			p[val_primi - 1] = local_n; // Aggiungo il valore primo appena trovato all'array dinamico
					
		}

		val_calcolati++;
	} 
} 



int main(int argc, char  *argv[])
{
	pthread_t th1, th2, th3;
	struct sigaction sa;

	int arg1 = 1, arg2 = 2, arg3 = 3; // Mi indicano con quale thread sto lavorando
	int tot_numeri; 
	int tot_primi;
	int fd;

	n = atoi(argv[1]); // Ho preso da riga di comando il numero totale di valori da testare
	printf("Avvio processo calcolatore su %d numeri...", n);
	tot_numeri = n;

	memset(&sa, '\0', sizeof(struct sigaction)); // Preparo la struttura che mi serve per interecttare la SIGPIPE
	sa.sa_handler = SIG_IGN; // Decido di ignorare la SIGPIPE

	// sigaction(int signum, const struct sigaction *act, struct sigaction *oldact); 
	sigaction(SIGPIPE, &sa, NULL); 

	/************************************************************************************************************
	 * Invece di creare tutto quel codice bash e due makefile distinti per settare la define DEBUG avrei potuto *
	 * semplicemente chiedere in input da qua se l'utente desiderasse usare DEBUG o meno e creare una variabile *
	 * debug = 0 o 1. In quel caso il codice sarebbe stato il seguente:                                         *
	 * 																											*
	
	printf("Si vuole usare la modalità debug?\nSi --> premere 1\nNo --> premere 0\n");

	//Controllo dell'input
	int DEBUG;
	while (scanf("%d",&DEBUG) != 1 || DEBUG < 0 || DEBUG > 1) // In input voglio solo 0 o 1 
	{
		printf("SCELTA NON CORRETTA! REINSERISCI IL VALORE: ");
		while (getchar() != '\n'); // Svuota il buffer di input ovvero elimina lo \n che si ha quando si preme invio
	}

	 *																											*
	 * 																											*
	 ************************************************************************************************************/


	// In arg ci ho messo il numero del calcolatore 
	pthread_create(&th1, NULL, (void *)calcolatore, (void *)&arg1); // Crea thread 1 
	pthread_create(&th2, NULL, (void *)calcolatore, (void *)&arg2); // Crea thread 2 
	pthread_create(&th3, NULL, (void *)calcolatore, (void *)&arg3); // Crea thread 3

	pthread_join(th1, NULL); 
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);

	
	printf("\n\n");


	printf("Connessione al memorizzatore...\n\n");

	if((fd = open(PIPE_PATH, O_WRONLY)) == -1) // Apro la pipe per comunicare con il memorizzatore
    	handle_errno_error("Errore apertura fd pipe");
    else
    	printf("Invio dati al memorizzatore in corso...\n\n");

    int tmp_primi_th1 = val_primi_th1;
	if(DEBUG) { blu(); printf("Array dinamico th1\n"); reset(); }
	for( ; tmp_primi_th1 > 0; tmp_primi_th1--) {
        //MOSTRO QUALI PRIMI HA TROVATO IL THREAD 1
        //printf("p[%d] : %d\n", tmp_primi_th1, *(p1+tmp_primi_th1)); // uguale a printf("p1[%d] : %d\n", tmp_primi_th1, p1[tmp_primi_th1]);

        //SCRITTURA SU PIPE -- MEMORIZZATORE
        if (write(fd, &p1[tmp_primi_th1 - 1], sizeof(int)) == -1) {
   			if (errno == EPIPE)
   				handle_errno_error("Il lettore ha chiuso la connessione... ");
   			else
  				handle_errno_error("Errore scrittura pipe");
    	}
    	if(DEBUG) { printf("Calcolatore: inviato "); green(); printf("%d\n", p1[tmp_primi_th1 - 1]); reset(); }
	}

    int tmp_primi_th2 = val_primi_th2;
    if(DEBUG) { blu(); printf("\n\nArray dinamico th2\n"); reset(); }
	for( ; tmp_primi_th2 > 0; tmp_primi_th2--) {
		//MOSTRO QUALI PRIMI HA TROVATO IL THREAD 2
        //printf("p[%d] : %d\n", tmp_primi_th2, *(p2+tmp_primi_th2)); // uguale a printf("p2[%d] : %d\n", tmp_primi_th2, p2[tmp_primi_th2]);

        //SCRITTURA SU PIPE -- MEMORIZZATORE
        if (write(fd, &p2[tmp_primi_th2 - 1], sizeof(int)) == -1) {
   			if (errno == EPIPE)
   				handle_errno_error("Il lettore ha chiuso la connessione... ");
   			else
  				handle_errno_error("Errore scrittura pipe");
    	}
    	if(DEBUG) { printf("Calcolatore: inviato "); green(); printf("%d\n", p2[tmp_primi_th2 - 1]); reset(); }
	}

	int tmp_primi_th3 = val_primi_th3;
	if(DEBUG) { blu(); printf("\n\nArray dinamico th3\n"); reset(); }
	for( ; tmp_primi_th3 > 0; tmp_primi_th3--) {
        //MOSTRO QUALI PRIMI HA TROVATO IL THREAD 3
        //printf("p[%d] : %d\n", tmp_primi_th3, *(p3+tmp_primi_th3)); // uguale a printf("p3[%d] : %d\n", tmp_primi_th3, p3[tmp_primi_th3]);

        //SCRITTURA SU PIPE -- MEMORIZZATORE
        if (write(fd, &p3[tmp_primi_th3 - 1], sizeof(int)) == -1) {
   			if (errno == EPIPE)
   				handle_errno_error("Il lettore ha chiuso la connessione... ");
   			else
  				handle_errno_error("Errore scrittura pipe");
    	}
    	if(DEBUG) { printf("Calcolatore: inviato "); green(); printf("%d\n", p3[tmp_primi_th3 - 1]); reset(); }

	}

    if(DEBUG) printf("\n\n");

	if (close(fd) == -1) // Chiudo la comunicazione con il memorizzatore
		handle_errno_error("Errore chiusura pipe nel calcolatore");
	else
		printf("Chiusura pipe nel calcolatore...\n\n");

	
    tot_primi = val_primi_th1 + val_primi_th2 + val_primi_th3;

    if(DEBUG) { printf("Su "); green(); printf("%d",tot_numeri); reset(); printf(" numeri, "); green(); printf("%d", tot_primi); reset(); printf(" sono primi\n\n"); }
    if(DEBUG) { printf("Valori primi trovati da th1 = "); green(); printf("%d\n", val_primi_th1); reset(); }
    if(DEBUG) { printf("Valori primi trovati da th2 = "); green(); printf("%d\n", val_primi_th2); reset(); }
    if(DEBUG) { printf("Valori primi trovati da th3 = "); green(); printf("%d\n", val_primi_th3); reset(); }

    red(); printf("\nPremi invio per terminare il processo\n"); reset();
    getchar(); //Do all'utente l'opportunità di leggere l'output a schermo sulla shell
    		   //Senza questo comando la shell si chiuderebbe automaticamente appena finisce il processo

	return EXIT_SUCCESS;
}