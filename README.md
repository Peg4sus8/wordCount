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
### Divisione del carico
