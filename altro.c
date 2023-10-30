#include <stdio.h>
#include <stdlib.h>

#include "mylib.h"

int primi(unsigned int n) {
	if(n <= 3)
		return 1;
  	else {
    	if ((n%2 == 0) || (n%3 == 0))
      		return 0;
    	else {
     		unsigned int i = 5;
      		while (i*i <= n) {
        		if((n%i == 0) || (n%(i+2) == 0))
          			return 0;
        		i = i + 6;
      		}
      		return 1;
   		}
  	}
}


int compara(const void *a, const void *b) {
    int n1 = *(int *)a;
    int n2 = *(int *)b;
    if(n1 > n2)
        return 1;
    else if(n1 < n2)
        return -1;
    else
        return 0;
}

// Attivo printf rossa
void red (void) {
    printf("\033[1;31m");
}

// Attivo printf verde
void green (void) {
    printf("\033[1;32m");
}

// Attivo printf blu
void blu (void) {
  printf("\033[1;34m");
}

// Resetto filtri printf
void reset (void) {
  printf("\033[0m");
}