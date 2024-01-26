/*
Group number-29
1.	Manvitha Aathmuri – R11847781 
2.	Raviteja Sirisanagandla - R11851152 
3.	Sai Tejaswini Chirumamilla - R11840694 
4.	Preetham Alwala - R11846544 
5.	Girish Gandham - R11870388 
6.	Ganesh Reddy Mannem - R11849144
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define INF 999

void printMatrix(int* matrix, int n);

int main(int argc, char** argv) {
    int n = 4; // Size of the graph
    int P = 4;  // Number of processes (P should be a square of a positive integer)
    int my_rank, num_procs;

    // MPI Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int local_n = n / sqrt(P); // Size of the sub-matrix

    // Create sub-matrices D and D0 for each process
    int* D = (int*)malloc(local_n * local_n * sizeof(int));
    int* D0 = (int*)malloc(local_n * local_n * sizeof(int));

    // Allocate additional arrays for intermediate calculations
    int* k_row = (int*)malloc(local_n * sizeof(int));

    // Initialize D and D0
    if (my_rank == 0) {
        printf("Enter the distance matrix (%d x %d):\n", n, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                scanf("%d", &D0[i * n + j]);
                if (i == j) {
                    D0[i * n + j] = 0;
                }
            }
        }
    }

    // Scatter D0 to all processes
    MPI_Scatter(D0, local_n * local_n, MPI_INT, D, local_n * local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform the Floyd-Warshall algorithm in parallel
    for (int k = 0; k < n; k++) {
        // Broadcast the k-th row from the root process to all processes
        int root = k / local_n;
        MPI_Bcast(k_row, local_n, MPI_INT, root, MPI_COMM_WORLD);

        for (int i = 0; i < local_n; i++) {
            for (int j = 0; j < local_n; j++) {
                int new_dist = D[i * local_n + k % local_n] + k_row[j];
                if (new_dist < D[i * local_n + j]) {
                    D[i * local_n + j] = new_dist;
                }
            }
        }
    }
    // Creating finalMAtrix for printing
    int* finalMatrix = NULL;
    if (my_rank == 0) {
        finalMatrix = (int*)malloc(n * n * sizeof(int));
    }

    // Gather D back to the root process for printing
    MPI_Gather(D, local_n * local_n, MPI_INT, finalMatrix, local_n * local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the final matrix on the root process
    if (my_rank == 0) {
        printMatrix(finalMatrix, n);
        free(finalMatrix);
    }

    // Free allocated memory
    free(D);
    free(D0);
    free(k_row);

    MPI_Finalize();

    return 0;
}

// Printing Matrix
void printMatrix(int* matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i * n + j] == INF) {
                printf("INF\t");
            } else {
                printf("%d\t", matrix[i * n + j]);
            }
        }
        printf("\n");
    }
}
