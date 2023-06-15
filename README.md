# wordCount
Un semplice programma che esegue il conteggio delle occorrenze di ogni parola all'interno di uno o più file in modo parallelo utilizzando OpenMPI e C
## Info generale
<i>Nome</i>: Vincenzo <br>
<i>Cognome</i>: Malafronte <br>
<i>Matricola</i>: 0522501522 <br>
<i>Sistema di benchmarking</i>: Cluster gcloud composto da 4 istanze di VM E2-standard-4 con 4 vCPU e 10GB di memoria <br>
## Indice
  1. Breve descrizione del problema
  2. Descrizione dell'implementazione
     * Divisione del carico
     * Conteggio delle occorrenze delle parole
     * Fusione dei risultati e ordinamento
  3. Info per l'esecuzione
  4. Test
  5. Benchmarking
     * Strong Scalability 
     * Weak Scalability
  6. Analisi dei risultati e conclusioni
## Breve descrizione del problema 
Il problema del Word Count riguarda il conteggio del numero di parole in un documento o in un file di testo generico. L'utilizzo del Word Counter può essere necessario quando un testo deve rimanere entro un determinato numero di parole come ad esempio: nel caso del mondo accademico, nei procedimenti legali, nell'ambito giornalistico o pubblicitario; può essere anche utilizzato dai traduttori per determinare il prezzo di un lavoro di traduzione.
E' stata fatta una semplice implementazione del Word Count in MPI durante il corso di "Programmazione Concorrente e Parallela sul Cloud" (PCPC - 0522500102) all'Università degli studi di Salerno.

L'obiettivo di questo progetto riguarda la creazione di un applicativo che realizzi il conteggio delle parole in modo parallelo tra più processi usando la libreria OpenMPI. Viene utilizzata questa implementazione per eseguire il conteggio delle parole su un gran numero di file; introduciamo il problema dividendolo in varie fasi:

1. Il processo MASTER avrà l'elenco di tutti i file da leggere all'interno di una directory. E' importante specificare che solo 1 dei processi leggerà l'elenco dei file. Quindi ciascuno dei processi riceverà la propria "parte" del file dal processo MASTER. Dopo che un processo ha ricevuto il suo elenco di file da elaborare, effettuerà quindi la lettura di ciascuno dei file per poi eseguire il conteggio delle parole, tenendo traccia ovviamente della frequenza con cui si verifica ogni parola trovata nei file. Chiameremo l'istogramma prodotto l'istogramma locale.
2. La seconda fase consiste nella produzione dell'istogramma locale che permetterà di combinare le frequenze delle parole per ogni processo. Ad esempio, la parola "ciao" potrebbe essere contata più volte in processi diversi, quindi alla fine si sommeranno tutte queste occorrenze.
3. L'ultima fase prevede che ciascuno dei processi invii i propri istogrammi locali al processo MASTER, che avrà solo bisogno di raccogliere tutte queste informazioni. Ovviamente ci saranno parole duplicate tra i processi, quindi il MASTER dovrebbe creare un file in formato .csv in ordine decrescente in base alla frequenza delle parole e, a parità di quest'ultima, in ordine alfabetico.
## Descrizione dell'implementazione
L'implementazione si basa sull'utilizzo di una HashMap(crediti a https://github.com/benhoyt/ht per l'implementazione), la quale permette di associare ad ogni <i>key</i>(la parola trovata), un <i>value</i>(le occorrenze) e che permette laccesso alle entry in tempo costante, il che aumenta le prestazioni. Per la risoluzione dei conflitti è stato adottato il principio di linear probing che funziona in questo modo: se si sta cercando di inserire un elemento ma ce n'è già uno, è sufficiente andare direttamente allo slot successivo, sfruttando il concetto di locality della cache, risultando più efficiente.
Verrà istanziata una HashTable per ogni processo, che conterrà ovviamente le parole e il numero di occorrenze che ogni processo è riuscito a trovare.
La soluzione si divide in vari passaggi:
* Come prima cosa il MASTER leggerà tutta la directory e ne prenderà tutte le informazioni necessarie per la lettura e la distribuzione, poi quindi calcolerà il carico per ogni processo e lo invierà in una struttura creata per la distribuzione.
* Una volta ricevuta la distribuzine ogni processore legge i file e ne conta le parole, nel caso in cui non legga il file per intero e, quindi, ci possa essere una parola a cavallo tra due processori, sraà il processore che ha già iniziato a leggerla ad inserirla nella propria table.
* Una volta contate le parole, ogni processo invia la propria tabella al MASTER che provvederà a unirle tutte e ordinarle per occorrenze e, in caso di stessa occorrenza, per ordine alfabetico.
* Finito il merge delle tabelle il MASTER provvede a scrivere i risultati su un file output<numtasks>.csv.
### Divisione del carico
Prima di iniziare lo sviluppo della soluzione è stato necessario definire come prendere i file da leggere e dove inserire le informaazioni relative ad esse. E' stata creta una struttura dati <code>Info</code> definita come mostrato di seguito, riempita dalla funzione <code>getInfo()</code> che va a leggere tutti i file nella directory <code>./files</code>. 
```c
typedef struct {            //informazioni sulla directory
    int n;                  //numero di file nella directory
    int *dims;              //dimensione di ogni file
    int *restDim;
    char **names;            //array dei nomi     
} Info;

Info getInfo(){   
    int numelem = 5;
    DIR *d;
    struct dirent *dir;
    struct stat buf;
    char path[DIRECTORY];
    Info i;
    i.n = 0;
    i.dims = malloc(numelem * sizeof(int));
    i.restDim = malloc(numelem * sizeof(int));
    i.names = malloc(numelem * MAX_NAME * sizeof(char));
    strcat(getcwd(path, DIRECTORY), "/files");
    chdir(path);

    if (!(d = opendir(path))) {
        fprintf(stderr, "[ERROR] Impossibile aprire la directory\n");
        exit(0);
    }
    while ((dir = readdir(d)) != NULL) {   
        stat(dir->d_name, &buf);
            
        if(!S_ISDIR(buf.st_mode))  {  
            i.names[i.n] = malloc(sizeof(char) * strlen(dir->d_name) + 1);
            strcpy(i.names[i.n], dir->d_name);
            i.dims[i.n] = buf.st_size;
            i.restDim[i.n] = buf.st_size; 
            i.n += 1;
        }

        if (i.n == numelem){
            numelem += 1;
            i.dims = realloc(i.dims, numelem * sizeof(int));
            i.restDim = realloc(i.restDim, numelem * sizeof(int));
            i.names = realloc(i.names, numelem * MAX_NAME * sizeof(char));
        }
    }
    closedir(d);
    return i;
}
```

Una volta presi dati relativi i files si è proceduto con il calcolo della distribuzione. Anche in questo caso è stata creata una struttura dati <code>DataDistr</code>, definita dal MASTER per ogni processore e poi inviata tramite una <code>MPI_Scatter</code>. La struttura contiene il numero di files assegnati al processore, poi tre array di interi che conterranno:
* l'indice del nome del file da leggere
* il fd da cui iniziare a leggere(relativo al file ovviamente)
* il fd in cui stopparsi.
Una volta definite le strutture utilizzate si può passare alla fase di distribuzione. Per distribuire i dati si utilizza la funzione <code>distribute()</code>. Questa funzione come prima cosa calcola il numero di bytes che ogni processore deve leggere, facendo una divisione della somma di tutti i files per tutti i processori, poi assegna l'approssimazione per difetto al master e l'approssimazione per eccesso a tutti gli altri processori. Presa la size, per ogni processore vengono definiti gli offset di inizio e fine per ogni processore, calcolati utililzzando la size definita prima. Di seguito la struttura dati con la funzione distribute() e setSize().

```c
typedef struct {            //da creare come array di nproc, ogni istanza
    int nFile;              //avrà le informazioni relative al carico di quel processo
    int indexFiles[SIZE];
    int startFd[SIZE];
    int endFd[SIZE];
    int size;
} DataDist;

void distribute(DataDist *struc, Info info, int numtasks){
    int somma = sommaDims(info.dims, info.n), 
        mediaFiles = myCeil((double)info.n/numtasks),
        count, tot;          //Contatore universale per i file in info 

    setSize(struc, somma, numtasks);
    //setSizeWS(struc, somma, numtasks);

    /*  ------------- Fine Distribzione dei valori ---------------- */
    /*  ----------------- Distribzione dei file ------------------- */
    /*
        Indici:
        i -> per i processori
        j -> per i file dei processori
        count -> per i file in Info
    */
    count = 0;

    for(int i = 0; i < numtasks ; i++){      //Ciclo per i processori 
        struc[i].nFile = 0;
        for(int j = 0; struc[i].size != 0 && count < info.n; j++){      //Ciclo per assegnare i file ai processori

            struc[i].indexFiles[j] = count;
            //  Start FD:
            if(info.restDim[count] == info.dims[count])     //Se il file ancora non è stato diviso
                struc[i].startFd[j] = 0;
            else                                        //Il file è gia stato diviso
                struc[i].startFd[j] = struc[i-1].endFd[struc[i-1].nFile - 1] + 1; //dimensione iniziale - dimensione rimanente + 1 = punto di partenza da cui leggere
            struc[i].size--; 
            info.restDim[count]--;
            
            //  EndFd
            if(struc[i].size >= info.restDim[count]){
                struc[i].endFd[j] = EOF;
                struc[i].size -= info.restDim[count];
                //somma += info.restDim[count];
                info.restDim[count] = 0;
                count++;
            } else {
                struc[i].endFd[j] = struc[i].startFd[j] + struc[i].size;
                info.restDim[count] -= struc[i].size;
                //somma += struc[i].size;
                struc[i].size = 0;
            }
            //  file aggiunto
            struc[i].nFile += 1;
        }   
    }
    setSize(struc, somma, numtasks); 
}

void setSize(DataDist *d, int sum, int numtasks){
    for (int j = 0; j < numtasks; j++){
        if(j != MASTER)
            d[j].size = myCeil((double)sum/numtasks);
        else
            d[j].size = myfloor((double)sum/numtasks);
    }
}
```
Con questo sono state definite le strutture dati e le funzioni utilizzate per la distribuzione del carico. Ora passiamo alla definizione del programma cheeseguirà il conteggio. Come prima cosa tutti i processi inizializzano una hashTable in cui inserire il conteggio delle parole e un array di stringhe in cui inserire i nomi dei files, il MASTER definisce un array di dataDistr di lunghezza <code>numtasks<\code>, nel quale inserirà i risultati della funzione <code>distribute()</code>, e crea un MPI_Datatype da usare per passare la distribuzione in modo più ottimale.
```C
ht *counts = ht_create();
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
```
Creato il datatype, il MASTER inizia a calcolare la distribuzione e la manda a tutti i processi tramite una <code>MPI_Scatter</code> e invia l'array dei nomi dei files con il numero di file totali nella directory tramite una <code>MPI_Bcast</code>
```C
if(myrank == MASTER){
		DataDist tmpDistr[numtasks];
		i = getInfo();

		for(int j = 0; j < i.n; j++)
			strcpy(names[j], i.names[j]);
		nfiles = i.n;
		distribute(tmpDistr, i, numtasks);
		MPI_Scatter(&tmpDistr[0], 1, MPI_DATA_DISTR, &mydistr, 1, MPI_DATA_DISTR, MASTER, MPI_COMM_WORLD);
		free(i.dims);
		free(i.names);
		free(i.restDim);
	}	else 
		MPI_Scatter(NULL, 1, MPI_DATA_DISTR, &mydistr, 1, MPI_DATA_DISTR, MASTER, MPI_COMM_WORLD);
		
	MPI_Bcast(&nfiles, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(names, nfiles * MAX_NAME, MPI_CHAR, MASTER, MPI_COMM_WORLD);

```




