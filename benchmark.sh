
make
export OMPI_MCA_btl_vader_single_copy_mechanism=none

mpirun --allow-run-as-root -np 1 --hostfile hfile ./main
mpirun --allow-run-as-root -np 1 --hostfile hfile ./main
mpirun --allow-run-as-root -np 1 --hostfile hfile ./main
mpirun --allow-run-as-root -np 2 --hostfile hfile ./main
mpirun --allow-run-as-root -np 2 --hostfile hfile ./main
mpirun --allow-run-as-root -np 2 --hostfile hfile ./main
mpirun --allow-run-as-root -np 3 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 3 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 3 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 4 --hostfile hfile ./main
mpirun --allow-run-as-root -np 4 --hostfile hfile ./main
mpirun --allow-run-as-root -np 4 --hostfile hfile ./main
mpirun --allow-run-as-root -np 5 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 5 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 5 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 6 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 6 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 6 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 7 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 7 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 7 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 8 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 8 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 8 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 9 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 9 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 9 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 10 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 10 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 10 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 11 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 11 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 11 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 12 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 12 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 12 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 13 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 13 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 13 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 14 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 14 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 14 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 15 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 15 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 15 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 16 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 16 --hostfile hfile ./main 
mpirun --allow-run-as-root -np 16 --hostfile hfile ./main

make clean
