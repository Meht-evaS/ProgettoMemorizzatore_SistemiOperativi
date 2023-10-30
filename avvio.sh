#!/bin/bash

if (($# != 2)); then
	echo "Usage: `basename $0`   n   path"
	exit 1
fi

echo -e "Leggere il README.txt se ancora non lo si è letto\n\n";

read -p "Premere 1 per attivare debug, 0 altrimenti : " SCELTA;

if test ! $SCELTA; then #Non è stato premuto ne 0 ne 1 quindi imposto di default 0
	echo -e "Non hai inserito nulla quindi DEBUG = 0\n"
	SCELTA=0;
fi

if (( $SCELTA == 0 )); then
	if test -f "makedebug1"; then #Guardo dal nome se l'ultima volta ho usato normal o debug
		mv makedebug1 makedebug0; #Se avevo usato debug ora la metto a normal
		
		RESULT=`echo $?`; 
		if (( $RESULT != 0 )); then #Controllo che il comando sia andato a buon fine
			echo -e "\nErrore: rinomino makedebug\n";
			exit 1;
		fi

		if test -f "makenormal"; then #Avvio makefile senza -DDEBUG
			make -f makenormal clean; #Sono costretto a eliminare gli eseguibili se nel mentre non erano stati aggiornati i file
			RESULT=`echo $?`;		  #Questo perchè se non sono stati aggiornati allora gli eseguibili sono già aggiornati appunto
			if (( $RESULT != 0 )); then	  #ma io ora li voglio compilare senza define di DEBUG ma makefile me lo impedisce
				echo -e "\nErrore: makenormal clean\n";
				exit 1;
			fi
			
			# Invece di usare il makefile clean potrei usare questo
			#if test -f "memorizzatore"; then
			#	rm memorizzatore;
			#fi
			#if test -f "calcolatori"; then
			#	rm calcolatori;
			#fi

			make -f makenormal normal;
			RESULT=`echo $?`; 
			if (( $RESULT != 0 )); then
				echo -e "\nErrore: makenormal normal\n";
				exit 1;
			fi
		else                          
			echo -e "\nErrore: makenormal non presente!\n";
			exit 1;
		fi
	else
		if test -f "makedebug0"; then #Faccio lavorare normalmente makefile per vedere se sono già aggiornati o meno
			make -f makenormal normal;#Qua non mi serve eliminare gli eseguibili in quanto anche la volta precedente avevo
			RESULT=`echo $?`; 		  #compilato senza la define del debug
			if (( $RESULT != 0 )); then
				echo -e "\nErrore: makenormal normal\n";
				exit 1;
			fi
		else                          
			echo -e "\nErrore: makedebug non presente!\n";
			exit 1;
		fi
	fi
else
	if (( $SCELTA == 1 )); then
		if test -f "makedebug0"; then #Guardo dal nome se l'ultima volta ho usato normal o debug
			mv makedebug0 makedebug1; #Se avevo usato normal ora la metto a debug
			
			RESULT=`echo $?`; 
			if (( $RESULT != 0 )); then 
				echo -e "\nErrore: rinomino makedebug\n";
				exit 1;
			fi

			make -f makedebug1 clean;
			RESULT=`echo $?`;
			if (( $RESULT != 0 )); then
				echo -e "\nErrore: makedebug clean\n";
				exit 1;
			fi

			# Invece di usare il makefile clean potrei usare questo
			#if test -f "memorizzatore"; then
			#	rm memorizzatore;
			#fi
			#if test -f "calcolatori"; then
			#	rm calcolatori;
			#fi

			make -f makedebug1 debug;
			RESULT=`echo $?`;
			if (( $RESULT != 0 )); then
				echo -e "\nErrore: makedebug debug\n";
				exit 1;
			fi
		else 
			if test -f "makedebug1"; then 
				make -f makedebug1 debug;
				RESULT=`echo $?`;
				if (( $RESULT != 0 )); then
					echo -e "\nErrore: makedebug debug\n";
					exit 1;
				fi
			else
				echo -e "\nErrore: makedebug non presente!\n";
				exit 1;
			fi
		fi
	fi 
fi


# Mi assicuro che nel filesystem del prof non esista una pipe creata precedentemente con il nome della mia pipe
# Lo faccio perchè se la pipe è piena, poi il memorizzatore mi andrebbe a leggere quei valori
# Serve solo alla prima esecuzione in assoluto poi diventa inutile
if test -p "/tmp/ipcPipe"; then
	rm /tmp/ipcPipe;
fi

# Creo la pipe prima di avviare i processi calcolatori e memorizzatore almeno non devo usare la sincronizzazione
# fra i due processi per dire quando viene creata la FIFO 
mkfifo /tmp/ipcPipe;

if test -p "/tmp/ipcPipe"; then
    chmod 666 /tmp/ipcPipe;
    echo -e "\n\nCreata pipe\n";
else
    echo -e "\nErrore creazione FIFO\n";
	exit 1;
fi


# Non mi ricordo se il programma che usa lei (prof) è in grado di aprire file senza estensione così faccio un 
# Controllo sul path/nome di input e gli aggiungo l'estensione .txt se non la aveva di già
if [[ $2 != *".txt" ]]; then
  	TMP="$2.txt";
else
	TMP="$2";
fi

# Controllo se il valore di n passatomi è un intero positivo, se si lo mando al programma
if test $1 -gt 0; then
	./memorizzatore $TMP 2>err_mem.log & x-terminal-emulator -e "./calcolatori $1 2>err_calc.log";	#Apro calcolatori in una shell differente		
    wait;
else
    echo -e "\nInput errato: il primo parametro deve essere un intero positivo";
  	exit 1;
fi


#In caso di errore nel memorizzatore non viene eliminata la pipe. In quel caso la elimino io.
if test -p "/tmp/ipcPipe"; then
	rm /tmp/ipcPipe;
fi