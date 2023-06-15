#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_NAME 35     //size massima del nome di un file
#define WORD 25         //size massima di una parola
#define SIZE 40         //numero di file massimi nella directory
#define MASTER 0
#define DIRECTORY 200   //numero di caratteri per scrivere la directory
#define ROW 4096

//  Definizione strutture 

typedef struct {            //da creare come array di nproc, ogni istanza
    int nFile;              //avrà le informazioni relative al carico di quel processo
    int indexFiles[SIZE];
    int startFd[SIZE];
    int endFd[SIZE];
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
extern Info getInfo();
extern void printInfo(Info i);
extern int sommaDims(int *dims, int n);
extern int myCeil(double d);
extern int myfloor(double d);
extern void printDistribution(DataDist *d, Info info, int numtasks);
extern void setSize(DataDist *d, int sum, int numtasks);
extern void printOneDistr(DataDist d, char names[][MAX_NAME], int rank);
extern void exit_nomem(void);
extern int ischar(char c);
extern int isAscii(char c);