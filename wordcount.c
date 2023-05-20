#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "datadistr.h"
#include "mpi.h"
#include "ht.h"

int main(int argc, char *argv[]){
	int myrank, numtasks;
	Info i;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	if(myrank == 0){
		DataDist x[numtasks];
		i = getInfo(".");
		
		distribute(x, i, numtasks);
		printDistribution(x, i, numtasks);
		
	}
	
	fflush(stdout);
	MPI_Finalize();
	return(0);
}

/*
		STEPS DEL PROGETTO:
		1) Lettura della direcctory e memorizzazione dei dati		(Fatto)
		2) Divisione del carico										(Fatto)
		3) Implementazione della Hash Table							()
		4) Conteggio delle occorrenze di ogni parola				()
		5) Fusione dei risultati									()
		
		Idea per (1)
		- Leggo la directory con la sua dimensione
		- Calcolo la dimensione(in Bytes) da assegnare ad ogni processore(Bytes / ntasks)
		- Inserisco le informazioni riguardanti i file da leggere per ogni processore all'interno di una struttura
		- La struttura del carico di lavoro conterrà tre array: uno di stringhe con i nomi dei file, uno di interi con gli fd di inizio
			uno di interi con gli offset di fine.
		- Ogni processore possiede un array con il numero di file che dovrà leggere ogni processore, questa gli verrà passata da MASTER tramite MPI_Send
		- Il primo offset di inizio/fine si troverà nella posizione (pidprecedenti*nfileprecedenti+1) 
		- FD ppuò avere 3 valori:
			- 0(inizia dall'inizio)
			- x(inizia/termina al byte x-esimo)
			- -1(leggi fino a eof)

		Idea per (2)
		- Ogni processore legge gli array di inizio/fine: se questi contengono un valore x allora si controlla il carattere iniziale o finale(in base all'array)
			per controllare se ci sono parole a cavallo tra i due file, nel caso inizia una comunicazione con il processore successivo(in caso
			in cui x sia nell'array di fine) o con il processore precedente(in caso in cui x sia nell'array di inizio)
		- Le parole con i count vengono inserite in una hash table
		- Alla fine della lettura elimino il file
		
*/