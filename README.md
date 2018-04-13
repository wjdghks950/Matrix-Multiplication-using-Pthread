# Matrix Multiplication using Pthread

The code in this repository does matrix multiplications using pthread. 

It compares the CPU time spent between two cases:

 1) No thread

 2) n threads

# Prerequisites

 gcc must be installed for compilation.

# Builds

$ gcc -o matmul MatMul_Threads.c -pthread

# Execution

$ ./matmul [dim. of matrix][no. of threads]
