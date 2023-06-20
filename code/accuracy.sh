make
mpirun --allow-run-as-root -np 1 ./main
mpirun --allow-run-as-root -np 2 ./main
mpirun --allow-run-as-root -np 3 ./main 
mpirun --allow-run-as-root -np 4 ./main
mpirun --allow-run-as-root -np 5 ./main 
mpirun --allow-run-as-root -np 6 ./main 
mpirun --allow-run-as-root -np 7 ./main 
mpirun --allow-run-as-root -np 8 ./main 
mpirun --allow-run-as-root -np 9 ./main 
mpirun --allow-run-as-root -np 10 ./main 
mpirun --allow-run-as-root -np 11 ./main 
mpirun --allow-run-as-root -np 12 ./main 
mpirun --allow-run-as-root -np 13 ./main 
mpirun --allow-run-as-root -np 14 ./main 
mpirun --allow-run-as-root -np 15 ./main 
mpirun --allow-run-as-root -np 16 ./main
mpirun --allow-run-as-root -np 17 ./main 
mpirun --allow-run-as-root -np 18 ./main 
mpirun --allow-run-as-root -np 19 ./main 
mpirun --allow-run-as-root -np 20 ./main 
mpirun --allow-run-as-root -np 21 ./main 
mpirun --allow-run-as-root -np 22 ./main 
mpirun --allow-run-as-root -np 23 ./main 
mpirun --allow-run-as-root -np 24 ./main 
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
diff oracle.csv output17.csv
diff oracle.csv output18.csv
diff oracle.csv output19.csv
diff oracle.csv output20.csv
diff oracle.csv output21.csv
diff oracle.csv output22.csv
diff oracle.csv output23.csv
diff oracle.csv output24.csv
cd ..
make clean
