#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "datadistr.h"
#include "mpi.h"

/*
typedef struct {    
    int nFile;
    int *indexFiles;
    int *startFd;
    int *endFd;
    int size;
} DataDist;

typedef struct {          
    int n;                
    int *dims;         
    int *restDim;   
    char **names;               
} Info;
*/

/*
    Si calcola la media come numero double, poi si assegna il floor della media
    (sarebbe la media approssimata per difetto) a MASTER, e si assegna il ceil
    della media(media approssimata per eccesso) a tutti gli altri processori.
*/
void distribute(DataDist *struc, Info info, int numtasks){
    int somma = sommaDims(info.dims, info.n), 
        mediaFiles = myCeil((double)info.n/numtasks),
        count, tot;          //Contatore universale per i file in info 

    
    setSize(struc, somma, numtasks);

    /*  ------------- Fine Distribzione dei valori ---------------- */
    /*  ----------------- Distribzione dei file ------------------- */
    /*
        Indici:
        i -> per i processori
        j -> per i file dei processori
        count -> per i file in Info
    */
    count = 0;
    //printf("somma iniziale: %d, size: %d\n", somma, struc[0].size);
    //tot = 0;


    for(int i = 0; i < numtasks ; i++){      //Ciclo per i processori 
        struc[i].nFile = 0;
       // somma = 0;
        for(int j = 0; struc[i].size != 0 && count < info.n; j++){      //Ciclo per assegnare i file ai processori

            struc[i].indexFiles[j] = count;
            //  Start FD:
            //printf("ciao %d \n", info.dims[count]); // alcune volte itera troppo ????????????????
            if(info.restDim[count] == info.dims[count])       //Se il file ancora non è stato diviso
                struc[i].startFd[j] = 0;
            else                                        //Il file è gia stato diviso
                struc[i].startFd[j] = struc[i-1].endFd[struc[i-1].nFile - 1] + 1; //dimensione iniziale - dimensione rimanente + 1 = punto di partenza da cui leggere
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
            /*if(struc[i].nFile == mediaFiles){
                mediaFiles += 1;
                struc[i].indexFiles = realloc(struc[i].indexFiles, mediaFiles * sizeof(int));
                struc[i].startFd = realloc(struc[i].startFd, mediaFiles * sizeof(int));
                struc[i].endFd = realloc(struc[i].endFd, mediaFiles * sizeof(int));
            }   */      

        }  
        
        //tot += somma;
        //printf("%d ) %d\n", i, somma);
        //printf("Size rimanente: %d\n", struc[i].size);
    }


    setSize(struc, somma, numtasks);
    //printf("Somma finale: %d\n", tot);
}

/*   
    Metodo che fa la lettura della directory e riempe la struttura Info con le 
    info che saranno poi utili per la distribuzione del carico ai processori  
*/
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

/*
    Metodo che stampa la struttura Info. Utilizzato per lo più in fase di 
    debugging.
*/
void printInfo(Info i){
    printf("\n --- La directory contiene %d files ---\n", i.n);
    printf("- La somma delle size è: %d bytes -\n", sommaDims(i.dims, i.n));
    for(int j = 0; j < i.n; j++){
        printf("%s - %d bytes[%d]\n", i.names[j], i.dims[j], i.restDim[j]);
    }
    printf("\n");
}

int sommaDims(int *dims, int n){
    int somma = 0;
    for (int i = 0; i < n; i++)
        somma += dims[i];
    
    return somma;
}

/*
    Metodo che simula la ceil di math.h(approssima per eccesso)
*/
int myCeil(double d){
    int x = (int)d;
    return ++x;
}

/*
    Metodo che simula la floor di math.h(approssima per difetto)
*/
int myfloor(double d){
    int x = (int)d;
    return d;
}
/*
    Metodo che stampa la struttura DataDistr. Utilizzato per lo più in fase di 
    debugging.
*/
void printDistribution(DataDist *d, Info info, int numtasks){
    int k;
    printf("Size: %d\n", d[0].size);
    for(int i = 0; i < numtasks; i++){
        printf("\n<-> sono %d e ho %d files\n", i, d[i].nFile);
        for(int j = 0; j < d[i].nFile; j++){
            k = d[i].indexFiles[j];
            printf(">> %s) - %d -> %d\n", info.names[k], d[i].startFd[j], d[i].endFd[j]);
        }
    }
    printInfo(info);
}

void setSize(DataDist *d, int sum, int numtasks){
    for (int j = 0; j < numtasks; j++){
        if(j != MASTER)
            d[j].size = myCeil((double)sum/numtasks);
        else
            d[j].size = myfloor((double)sum/numtasks);
    }
}

void printOneDistr(DataDist d, char names[][MAX_NAME], int rank){
    printf("\t --- sono %d ---\n", rank);
    for (int i = 0; i < d.nFile; i++)
        printf(">> %s: start[%d] end[%d]\n", names[d.indexFiles[i]], d.startFd[i], d.endFd[i]);
}

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

int ischar(char c){
    int ch = (int) c;
    return (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122);
}

int isAscii(char c){
	int ch = (int) c;
	return (ch <= 255 && ch >= 0);
}