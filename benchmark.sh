
make
export OMPI_MCA_btl_vader_single_copy_mechanism=none

mpirun --allow-run-as-root -np 1 ./main
mpirun --allow-run-as-root -np 1 ./main
mpirun --allow-run-as-root -np 1 ./main
mpirun --allow-run-as-root -np 2 ./main
mpirun --allow-run-as-root -np 2 ./main
mpirun --allow-run-as-root -np 2 ./main
mpirun --allow-run-as-root -np 3 ./main 
mpirun --allow-run-as-root -np 3 ./main 
mpirun --allow-run-as-root -np 3 ./main 
mpirun --allow-run-as-root -np 4 ./main
mpirun --allow-run-as-root -np 4 ./main
mpirun --allow-run-as-root -np 4 ./main
mpirun --allow-run-as-root -np 5 ./main 
mpirun --allow-run-as-root -np 5 ./main 
mpirun --allow-run-as-root -np 5 ./main 
mpirun --allow-run-as-root -np 6 ./main 
mpirun --allow-run-as-root -np 6 ./main 
mpirun --allow-run-as-root -np 6 ./main 
mpirun --allow-run-as-root -np 7 ./main 
mpirun --allow-run-as-root -np 7 ./main 
mpirun --allow-run-as-root -np 7 ./main 
mpirun --allow-run-as-root -np 8 ./main 
mpirun --allow-run-as-root -np 8 ./main 
mpirun --allow-run-as-root -np 8 ./main 
mpirun --allow-run-as-root -np 9 ./main 
mpirun --allow-run-as-root -np 9 ./main 
mpirun --allow-run-as-root -np 9 ./main 
mpirun --allow-run-as-root -np 10 ./main 
mpirun --allow-run-as-root -np 10 ./main 
mpirun --allow-run-as-root -np 10 ./main 
mpirun --allow-run-as-root -np 11 ./main 
mpirun --allow-run-as-root -np 11 ./main 
mpirun --allow-run-as-root -np 11 ./main 
mpirun --allow-run-as-root -np 12 ./main 
mpirun --allow-run-as-root -np 12 ./main 
mpirun --allow-run-as-root -np 12 ./main 
mpirun --allow-run-as-root -np 13 ./main 
mpirun --allow-run-as-root -np 13 ./main 
mpirun --allow-run-as-root -np 13 ./main 
mpirun --allow-run-as-root -np 14 ./main 
mpirun --allow-run-as-root -np 14 ./main 
mpirun --allow-run-as-root -np 14 ./main 
mpirun --allow-run-as-root -np 15 ./main 
mpirun --allow-run-as-root -np 15 ./main 
mpirun --allow-run-as-root -np 15 ./main 
mpirun --allow-run-as-root -np 16 ./main 
mpirun --allow-run-as-root -np 16 ./main 
mpirun --allow-run-as-root -np 16 ./main

make clean
