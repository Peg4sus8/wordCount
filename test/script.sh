make
mpirun --allow-run-as-root -np 1 ./test
mpirun --allow-run-as-root -np 2 ./test
mpirun --allow-run-as-root -np 3 ./test 
mpirun --allow-run-as-root -np 4 ./test
mpirun --allow-run-as-root -np 5 ./test 
mpirun --allow-run-as-root -np 6 ./test 
mpirun --allow-run-as-root -np 7 ./test 
mpirun --allow-run-as-root -np 8 ./test 
mpirun --allow-run-as-root -np 9 ./test 
mpirun --allow-run-as-root -np 10 ./test 
mpirun --allow-run-as-root -np 11 ./test 
mpirun --allow-run-as-root -np 12 ./test 
mpirun --allow-run-as-root -np 13 ./test 
mpirun --allow-run-as-root -np 14 ./test 
mpirun --allow-run-as-root -np 15 ./test 
mpirun --allow-run-as-root -np 16 ./test 
cd output
diff oracle.csv output1.csv
diff oracle.csv output2.csv
diff oracle.csv output3.csv
diff oracle.csv output4.csv
diff oracle.csv output5.csv
diff oracle.csv output6.csv
diff oracle.csv output7.csv
diff oracle.csv output8.csv
diff oracle.csv output9.csv
diff oracle.csv output10.csv
diff oracle.csv output11.csv
diff oracle.csv output12.csv
diff oracle.csv output13.csv
diff oracle.csv output14.csv
diff oracle.csv output15.csv
diff oracle.csv output16.csv
cd ..
make clean
