# Parallel-Programming
This repository contains different Parallel processing solutions to hot plate problem.

The hot plate problem is to determine the temperature distribution on a two-dimensional plate with constant boundary conditions. The elements on the boundaries have fixed temperatures. The elements on the bottom row have a fixed temperature of 100 degrees. The elements on the top row and leftmost and rightmost columns have temperature of zero degrees. Additionally, row 400 columns 0 through 330 are fixed at 100 degrees. A cell at row 200, column 500 also is fixed at 100 degrees. All other cells start with a temperature of 50 degrees. The job of the algorithm is to find the steady-state temperature distribution of the interior elements. Steady state means that the temperature of any cell is equal to the average of the temperatures of its four neighbors on the north, south, east, and west directions.

The solution is parallelized using OpenMP, MPI, Pthreads and CUDA.
