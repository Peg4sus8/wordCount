#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_NAME 25
#define SIZE 17     //size massima di una parola
#define MASTER 0
//  Definizione strutture 

typedef struct {            //da creare come array di nproc, ogni istanza
    int nFile;              //avrà le informazioni relative al carico di quel processo
    int *indexFiles;
    int *startFd;
    int *endFd;
    int size;
} DataDist;


typedef struct {            //informazioni sulla directory
    int n;                  //numero di file nella directory
    int *dims;              //dimensione di ogni file
    int *restDim;
    char **names;            //array dei nomi     
} Info;

//  Definizione metodi 

extern void distribute(DataDist *struc, Info info, int numtasks);
extern Info getInfo(char *path);
extern void printInfo(Info i);
extern int sommaDims(int *dims, int n);
extern int myCeil(double d);
extern int myfloor(double d);
extern void printDistribution(DataDist *d, Info info, int numtasks);
extern void setSize(DataDist *d, int sum, int numtasks);