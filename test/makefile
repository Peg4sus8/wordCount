test: wordcount-tester.o ht.o datadistr.o 
	mpicc -o test ht.o datadistr.o wordcount-tester.o -lm

wordcount-tester.o: wordcount-tester.c ht.h datadistr.h 
	mpicc -c wordcount-tester.c -lm

datadistr.o: datadistr.c datadistr.h
	mpicc -c datadistr.c -lm

ht.o: ht.c ht.h
	mpicc -c ht.c -lm

clean:
	-rm test *.o