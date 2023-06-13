#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include "ht.h"
#include "datadistr.h"
#include "mpi.h"

void insertWord(ht* counts, char *temp, int countWord);


int main(int argc, char *argv[]){
	//Variabili generali per sitribuzione e lettura parole 
	int myrank, numtasks, nfiles, c=0;
	double start, middle, end;
	char names[SIZE][MAX_NAME], *file, *path, *fpath, ch;
	DataDist mydistr;
	Info i;
	ht *counts = ht_create(),
	   *mergedTable;
	FILE *fp;
	MPI_Status status;

	//variabili per MPI_PACK
	int wordlenght = WORD,
		namelenght = MAX_NAME,
		direclenght = DIRECTORY, 
		sizepack, countpack, position, sizetoreceive, numel, 
		*countselem, *dispelem;
	char *hashsend, *htsreceived;
	hti it, htit;

	//variabili per la scrittura
	char *outputfile, *ntask;
	
	//Inizializzazione
	//counts = ht_create();
	if (counts == NULL)
        exit_nomem();
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	file = malloc(namelenght * sizeof(char) + direclenght * sizeof(char));
	path = malloc(direclenght * sizeof(char));
	fpath = malloc(direclenght * sizeof(char));
	getcwd(path, direclenght);
	strcpy(fpath, path);
    strcat(fpath, "/files/");
	
	//---------------- Creo il datatype per la distribuzione dei dati ----------------------------	
    MPI_Datatype MPI_DATA_DISTR;
    int lengths[5] = {1, SIZE, SIZE, SIZE, 1};
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint base_address, displacements[5];
    DataDist dummyDataDistribution;

    MPI_Get_address(&dummyDataDistribution, &base_address);
    MPI_Get_address(&dummyDataDistribution.nFile, &displacements[0]);
    MPI_Get_address(&dummyDataDistribution.indexFiles, &displacements[1]);
    MPI_Get_address(&dummyDataDistribution.startFd, &displacements[2]);
    MPI_Get_address(&dummyDataDistribution.endFd, &displacements[3]);
    MPI_Get_address(&dummyDataDistribution.size, &displacements[4]);

    for(int i = 0; i < 5; i++)
        displacements[i] = MPI_Aint_diff(displacements[i], base_address);

    MPI_Type_create_struct(5, lengths, displacements, types, &MPI_DATA_DISTR);
    MPI_Type_commit(&MPI_DATA_DISTR);
	//--------------------------------- Fine ------------------------------------------------------	
	
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();
	
	if(myrank == MASTER){
		DataDist tmpDistr[numtasks];
		i = getInfo();

		for(int j = 0; j < i.n; j++)
			strcpy(names[j], i.names[j]);
		nfiles = i.n;
		distribute(tmpDistr, i, numtasks);
		
		//printDistribution(tmpDistr, i, numtasks);
		
		MPI_Scatter(&tmpDistr[0], 1, MPI_DATA_DISTR, &mydistr, 1, MPI_DATA_DISTR, MASTER, MPI_COMM_WORLD);
		free(i.dims);
		free(i.names);
		free(i.restDim);
	}	else 
		MPI_Scatter(NULL, 1, MPI_DATA_DISTR, &mydistr, 1, MPI_DATA_DISTR, MASTER, MPI_COMM_WORLD);
		
	MPI_Bcast(&nfiles, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(names, nfiles * MAX_NAME, MPI_CHAR, MASTER, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	middle = MPI_Wtime() - start;
	//printOneDistr(mydistr, names, myrank);
	
/*
typedef struct {    
    int nFile;
    int *indexFiles;
    int *startFd;
    int *endFd;
    int size;
} DataDist;
*/
	// ---------------------- Inizio conteggio parole ---------------------------------------
	for(int i = 0; i < mydistr.nFile; i++){
		char wordToAdd[WORD];
		int j = 0;
		strcpy(file, fpath);
		strcat(file, names[mydistr.indexFiles[i]]);		

		if (!(fp = fopen(file, "r"))) {
            fprintf(stderr, "[%d](%s)Error: Failed to open entry file - %s\n", myrank, file, strerror(errno));
            fclose(fp);
            return 1;
        } else
		
		if(mydistr.startFd[i] != 0)	{		//controlla se il carattere precedente è un char
			fseek(fp, mydistr.startFd[i] - 1, SEEK_SET);
			if(ischar(fgetc(fp))){			//se è un char allora skippa la parola
				fseek(fp, -2, SEEK_CUR);
				while(ischar(fgetc(fp)))
					;
			}
		}	
		
		if(mydistr.endFd[i] == EOF){
			while((ch = fgetc(fp)) != EOF){
				c++;
				if(ischar(ch)){
					wordToAdd[j++] = ch;
				} else {
					wordToAdd[j] = '\0';
					if(strlen(wordToAdd) > 0){
						insertWord(counts, wordToAdd, 1);
					}
					strcpy(wordToAdd, "");
					j = 0;
				}
			}	
		} else {
			while(1){
				ch = fgetc(fp);
				c++;
				//if(ftell(fp) >= 142 && ftell(fp)<=152)	printf("%c\n", ch);	

				if(ftell(fp) == mydistr.endFd[i]) {	//controlla che la parola appena letta sia terminata
					if(ischar(ch)){
						wordToAdd[j++] = ch;
						while(1){
							ch = fgetc(fp);
							c++;
							if(ischar(ch))
								wordToAdd[j++] = ch;
							else {
								wordToAdd[j] = '\0';
								if(strlen(wordToAdd) > 0){						
									insertWord(counts, wordToAdd, 1);
								}
								break;
							}
						}		
					} else{
						wordToAdd[j] = '\0';
						if(strlen(wordToAdd) > 0){						
							insertWord(counts, wordToAdd, 1);
						}
					}
					break;			
				}

				if(ischar(ch)){
					wordToAdd[j++] = ch;
				} else {
					wordToAdd[j] = '\0';
					if(strlen(wordToAdd) > 0){						
						insertWord(counts, wordToAdd, 1);
					}
					strcpy(wordToAdd, "");
					j = 0;
				}
			}
		}
		fclose(fp);
	}
	printf("%d, %d\n", myrank, c);
	//MPI_Barrier(MPI_COMM_WORLD);
	
	// -------- Fine conteggio parole ----- Inizio spedizione tabelle --------
	sizepack = sizeof(int) + (((sizeof(char) * wordlenght) + sizeof(int)) * ht_length(counts));
	countpack = 4 + ((wordlenght + 4) * ht_length(counts));
	position = 0;
	numel = ht_length(counts) * (wordlenght + 4) + 4;
	it = ht_iterator(counts);
	hashsend = malloc(sizepack);
	//printf("(%d)>> %ld\n", myrank, ht_length(counts));
	
	if(myrank == MASTER){	
		countselem  = malloc(numtasks * sizeof(int));
		dispelem = malloc(numtasks * sizeof(int));
	}

	MPI_Gather(&numel, 1, MPI_INT, countselem, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

	if(myrank == MASTER){	//se sei master preparati a ricevere la gatherv
		int tmp = 0;
		dispelem[0] = 0;
		sizetoreceive += countselem[0]; 	
		
		for(int i = 1; i < numtasks; i++){
			dispelem[i] = tmp + countselem[i-1];
			tmp += countselem[i-1];
			sizetoreceive += countselem[i];
		}
		htsreceived = malloc((sizetoreceive + 1) * sizeof(char));	
	} else {	//altrimenti impacchettate i dati
		int length = ht_length(counts);
		MPI_Pack(&length, 1, MPI_INT, hashsend, sizepack, &position, MPI_COMM_WORLD);
		for(int i = 0; i < length; i++){
			ht_next(&it);
			int countword = *((int*) it.value);
			char key[wordlenght];
			strcpy(key, it.key);
			//printf("%s - %d\n", key, countword);
			MPI_Pack(key, wordlenght, MPI_CHAR, hashsend, sizepack, &position, MPI_COMM_WORLD);
			MPI_Pack(&countword, 1, MPI_INT, hashsend, sizepack, &position, MPI_COMM_WORLD);
		}
	}
	MPI_Gatherv(hashsend, countpack, MPI_PACKED, htsreceived, countselem, dispelem, MPI_PACKED, MASTER, MPI_COMM_WORLD);

	// ----------- Fine spedizione tabelle ----- Inizio unpack ---------------
	
	if(myrank == MASTER){
        int numEntries;
        for(int i = 1; i < numtasks; i++){
            position = 0;
            MPI_Unpack(&htsreceived[dispelem[i]], countselem[i], &position, &numEntries, 1, MPI_INT, MPI_COMM_WORLD);
            int countWord;
            char temp[wordlenght];
            for(int j = 0; j < numEntries; j++){
                MPI_Unpack(&htsreceived[dispelem[i]], countselem[i], &position, temp, wordlenght, MPI_CHAR, MPI_COMM_WORLD);
                MPI_Unpack(&htsreceived[dispelem[i]], countselem[i], &position, &countWord, 1, MPI_INT, MPI_COMM_WORLD);
                insertWord(counts, temp, countWord);
            }	
        }

		//Ordinamento
		numEntries = ht_length(counts);
        merged_ht* mergedTable = merged_ht_create(numEntries);

        if (mergedTable == NULL)
            exit_nomem();

        // Struttura dati che permettere di scorrere gli elementi della Hash Table
        htit = ht_iterator(counts);
        int q = 0;

        // Riempio gli array per riordinarli (per evitare di toccare la HashTable)
        while (ht_next(&htit)) {
			setword(mergedTable, htit.key, q);
            //strcpy(&mergedTable[q].word, htit.key);
            mergedTable[q].freq = *(int*) htit.value;
            q++;
        }

        // Effettuo l'ordinamento
        mergeSort(mergedTable, 0, numEntries - 1);

		/*	-------		Fase di scrittura dei risultati nel file		------	*/
		//creo il file di output
		ntask = malloc(sizeof(char));
		outputfile = malloc(namelenght * sizeof(char));
		sprintf(ntask, "%d", numtasks);
		strcpy(outputfile, "output");
		strcat(outputfile, ntask);
		strcat(outputfile, ".csv");

		// Prendo la directory nella quale verrà stampato l'output
        strcpy(fpath, path);
        strcat(fpath, "/output/");
        strcat(fpath, outputfile);

        // Apro il file per scriverci dentro
        if (!(fp = fopen(fpath, "w"))) {
			printf("\t ------------ OPS ------------ \n");
            fprintf(stderr, "Error: Failed to open entry file - %s\n", strerror(errno));
            fclose(fp);
            return 1;
        }

        // Viene scritto l'output in un file
        fprintf(fp, "PAROLA,FREQUENZA\n");
        for(int i = 0; i < numEntries; i++)
            fprintf(fp, "%s,%d\n", mergedTable[i].word, mergedTable[i].freq);

        fclose(fp);

        free(countselem);
        free(dispelem);
        free(htsreceived);
        merged_ht_destroy(mergedTable);
	}

	// --------------------------------- Fine ---------------------------------
	MPI_Barrier(MPI_COMM_WORLD);
	end = MPI_Wtime() - start;
	
	if(myrank == MASTER){
		printf("Tempo di calcolo/distribuzione: %f\n", middle);
		printf("Tempo totale: %f\n", end);
	}
	fflush(stdout);

	free(hashsend);

	MPI_Finalize();
	return(0);
}

/*
		STEPS DEL PROGETTO:
		1) Lettura della direcctory e memorizzazione dei dati		(Fatto)
		2) Divisione del carico										(Fatto)
		3) Implementazione della Hash Table							(Fatto)
		4) Conteggio delle occorrenze di ogni parola				(Fatto)
		5) Fusione dei risultati									(Fatto)
		6) merge													()
		
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

		Idea per (4)
		- Ogni processore apre i propri file e procede alla lettura riga per riga e alla classificazione delle parole.
		- Se l'inizio(startFd) non è 0 allora si procede con il controllo del carattere:
		   - Se il carattere iniziale è un char allora si legge il carattere precedente: se è anch'esso un char signfica che ci troviamo in presenza di una parola 
			 divisa tra due processori, si occuperà il processore precedente a segnarla
		   - Se il carattere non è una lettera allora non c'è nessuna parola divisa e si può iniziare il conteggio delle parole
		- Se la fine(endFd) non è EOF allora potrebbe esserci una parola divisa tra due processori, quindi si procede con il controllo  
		
*/

void insertWord(ht* counts, char *temp, int countWord){
    size_t len = strlen(temp);
    char *lower = calloc(len+1, sizeof(char));
	
    for (size_t i = 0; i < len; ++i) {
        lower[i] = tolower((unsigned char) temp[i]);
    }

    void* value = ht_get(counts, lower);
    if (value != NULL) {
        // Already exists, increment int that value points to.
        int* pcount = (int*) value;
        (*pcount) += countWord;
    } else {
        // Word not found, allocate space for new int and set to countWord.
        int* pcount = malloc(sizeof(int));
        if (pcount == NULL) {
            exit_nomem();
        }
        *pcount = countWord;
        if (ht_set(counts, lower, pcount) == NULL) {
            exit_nomem();
        }
    }
}

