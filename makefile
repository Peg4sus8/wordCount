main: ht.o datadistr.o wordcount.o 
	mpicc -o main ht.o datadistr.o wordcount.o

wordcount.o: wordcount.c datadistr.h ht.h
	mpicc -c wordcount.c

datadistr.o: datadistr.c datadistr.h 
	mpicc -c datadistr.c

ht.o : ht.c ht.h
	mpicc -c ht.c

clean:
	-rm main ht.o datadistr.o wordcount.o