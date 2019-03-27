# Algoritmo di Cooper implementato in C

## Introduzione
Questo repository contiene la mia implementazione di una versione ridotta
dell'algoritmo di eliminazione dei quantificatori nell'aritmetica di Presburger
svolto come progetto in Università.

## Requisiti
Per compilare il programma è sufficiente `gcc`, ciononostante rende molto più
agevole l'esecuzione dell'ultimo utilizzare il `makefile` che ho creato
appositamente.
Inoltre il `makefile` permette di effettuare altre utili operazioni, per le quali
però sono necessari i seguenti:

* `gdb` per il debugging
* `guile` per valutare le espressioni generate dal programma
* `python` (**versione 3**) per generare il sorgente che viene eseguito da `yices`
* `yices` (**versione 1**) per verificare la soddisfacibilità delle espressioni
* generate dal programma
* `valgrind` per appurare la mancanza di memory leaks


## Utilizzo
Sono disponibili i seguenti comandi:
* `make run` esegue il programma usando dando come input la formula e la
variabile da eliminare impostate nel `makefile`
* `make valgrind` esegue il programma con `valgrind` con gli stessi input di `make run`
* `make debug` esegue il programma con `gdb` con gli stessi input di `make run`
* `make sat` esegue il programma con gli stessi input di `make run` e genera un
sorgente per `yices` contenenta la formula equivalente, successivamente esegue
tale sorgente con `yices` verifcandone la soddisfacibilità.
* `make eval` esegue il programma con gli stessi input di `make run` e cerca di
valutare la formula equivalente, ovviamente ciò funziona solo se la formula
equivalente non contiene più variabili


## Documentazione
L'[elaborato](doc/elaborato.pdf) che si trova all'interno di `/doc/`
contiene una breve introduzione teorica , la discussione
dell'implementazione e alcune informazioni sull'utilizzo (tra cui esempi).


