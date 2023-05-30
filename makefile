main: wordcount.o ht.o datadistr.o 
	mpicc -o main ht.o datadistr.o wordcount.o -lm

wordcount.o: wordcount.c ht.h datadistr.h 
	mpicc -c wordcount.c -lm

datadistr.o: datadistr.c datadistr.h
	mpicc -c datadistr.c -lm

ht.o: ht.c ht.h
	mpicc -c ht.c -lm

clean:
	-rm main *.o