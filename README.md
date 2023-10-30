# Progetto del Laboratorio di Sistemi Operativi

Progetto d'esame, in linguaggio `C` e `bash`, per la materia Laboratorio di Sistemi Operativi (A.A. 2020/2021).

##  :pencil: Sommario
* [Obiettivo](#obiettivo)
* [Ambiente di lavoro](#ambiente-di-lavoro)
* [Setup](#setup)
* [Scelte progettuali](#scelte-progettuali)
* [Appunti personali](#appunti-personali)

## :dart: Obiettivo <a name="obiettivo"/>
L'obiettivo di questo progetto è la realizzazione di un'applicazione che rispetti una serie di vincoli che è possibile leggere all'interno del file [ProgettoLabSO_a.a.2020-21.pdf](ProgettoLabSO_a.a.2020-21.pdf). In generale ci viene chiesto di realizzare un'applicazione in linguaggio `C` che verifichi quali tra i primi `n` numeri naturali siano primi. La verifica deve essere attuata **in parallelo** da almeno 3 entità di calcolo distinte, che possono essere sia processi single-threaded che threads di uno stesso processo, che vengono chiamate "*calcolatori*". I numeri risultati primi (e solo questi) devono essere memorizzati in un file uno per ogni riga ed in ordine crescente; tale memorizzazione deve essere attuata da un altro processo **completamente separato** dai calcolatori che chiameremo "*memorizzatore*". Il memorizzatore viene informato dagli altri processi (calcolatori o accessori) di quali numeri primi registrare tramite almeno una delle seguenti forme di `IPC`: `pipe`, `socket`, `signal`, `shared memory`.

## :computer: Ambiente di lavoro <a name="ambiente-di-lavoro"/>
Il progetto è stato realizzato su `Ubuntu 18.04 LTS`.

## :gear: Setup <a name="setup"/>
Per eseguire questo progetto, dopo aver scaricato i file di questo repository, basta eseguire i seguenti comandi:
```bash
sudo chmod +x avvio.sh
./avvio.sh n path
```

dove:
- **n**: numeri naturali (da `0` a `n`) con cui vogliamo lavorare
- **path**: path/to/file su cui far salvare al memorizzatore i numeri primi trovati

A schermo poi, interattivamente, verranno richieste alcune altre scelte.


## Scelte progettuali  

**Modalità DEBUG**:<br> 
Si tratta di una `define` che se definita durante la compilazione permette di ottenere a schermo molte più informazioni su quello che il processo sta facendo in quell'istante. Ho deciso di metterla in quanto a schermo vengono già stampate tutte le informazioni essenziali richieste dalla consegna del progetto ma, grazie a questa modalità, si capisce molto meglio ciò che sta avvenendo. Purtroppo però, andando a stampare a schermo moltissime cose, il programma si rallenta molto e quindi ho deciso di rendere opzionale il suo utilizzo (anche se personalmente la terrei sempre attiva).

Nel progetto ho scelto di usare una sola `mutex` che serve per coordinare i vari `thread` nel reperire il successivo numero di cui testare la primalità. Ho deciso di non usarne una seconda, che sarebbe dovuta stare sempre all'interno dei thread e che andava a regolare l'accesso alla `pipe` per inviare i dati direttamente al `memorizzatore`, in quanto (inizialmente l'avevo implementata) andava a rallentare notevolmente il funzionamento del programma. Per questo ho deciso di salvare tutti i numeri primi trovati da ciascuno thread in array dinamici che poi posso usare dal main e inviarli al memorizzatore.

Essendo che vado a decrementare la variabile `n`, i test di primalità vanno dal valore più alto (`n`) fino al valore più basso (`1`). Salvandoli anche in questo ordine, ho deciso però che al memorizzatore glie li invio in verso opposto così da avere, anche se minimamente, già un buon livello di ordinamento e ridurre il tempo di lavoro di `quicksort`.

Inoltre ho deciso anche che, mio malgrado, è meglio non fare `printf` colorate finchè si sta dentro ai thread in quanto esse non sono `thread safe` e molte volte, mentre un thread aveva appena utilizzato la funzione per colorare la shell, esso veniva deschedulato e un'altro thread prendeva il suo posto ma quando andava a stampare aveva un colore che non gli apparteneva.     

Inoltre ho preferito che i due processi (memorizzatore e calcolatori) venissero eseguiti da due shell differenti in quanto, se i loro output venissero stampati sulla stessa shell, non si capirebbe bene chi sta scrivendo cosa o comunque il risultato non sarebbe minimamente apprezzabile a livello estetico (soprattutto con la `modalità DEBUG` attivita).

Purtroppo, per poterli eseguire su shell differenti, non ho trovato nessun comando che mantenesse la seconda shell attiva dopo la fine del processo eseguito in essa. Allora ho dovuto usare una specie di scappatoia chiedendo all'utente di teminare il processo (premendo `INVIO`) solo dopo che ha finito di leggere tutto ciò che gli interessa. 

Infine, dato che sui calcolatori, se vi è un errore, la shell si chiude subito, per vedere l'output lo ho rediretto (`STDERR`) in un file chiamato `err_calc.log` . La stessa cosa l'ho fatta anche per il memorizzatore anche se a schermo si vede già tutto.

## Appunti personali  

PREMETTO CHE:<br>
Avendo avuto un pò di tempo libero ho deciso volontariamente di "complicarmi" la vita e di andare a usare principalmente i comandi bash invece che quelli C in quanto volevo esercitarmi di più in quell'ambito. Di seguito quindi troverete la spiegazione del metodo alternativo (anche se più 
dispendioso) per l'implementazione della modalità `DEBUG`.

Se si usa il `makefile` ho notato che a lui non fa differenza se si passa o meno la define di `DEBUG`. Nel senso che se i file di dipendenza non hanno subito cambiamenti, per lui non fa differenza che tu ora voglia usare una define o meno per compilare...non te lo farà fare. Quindi, per poterla implementare correttamente devo innanzitutto chiedere all'utente se vuole usare il `DEBUG` o meno. Se si, devo sfruttare il file `avvio.sh` e ogni volta andare a controllare se la volta precedente avevo usato la modalità `DEBUG` o meno. Se l'avevo usata (rispettivamente non l'avevo usata) e ora la voglio usare nuovamente (rispettivamente non la voglio usare nuovamente), mi basta lanciare il `makefile makedebug1` (rispettivamente `makenormal`). Se invece la volta prima avevo usato il `DEBUG` mentre ora non lo voglio usare (rispettivamente la volta prima non lo avevo usato mentre ora lo voglio usare) devo andare a rimuovere i vecchi eseguibili, modificare il nome del makefile da `makedebug1` a `makedebug0` (sul nome uso `0` e `1` per indicare se la volta prima ho usato o meno il debug, `0` non l' ho usato, `1` l'ho usato) e lanciare il `makefile makenormal` (rispettivamente devo rimuovere i vecchi eseguibili, modificare il nome del makefile da `makedebug0` a `makedebug1` e lanciare il `makefile makedebug1`).

Invece di fare tutto questo avrei potuto più semplicemente chiedere, dal programma C, un input all'utente per settare una variabile intera `DEBUG` a `1` o `0` e che mi andava a svolgere lo stesso medesimo lavoro della `define DEBUG`.
