#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define generate_data(i, j) (i) + (j) * (j)

int main(int argc, char **argv) {
    // Initialize variables
    int i, j, pid, np, mtag, count;
    double t0, t1;
    int data[100][100], row_sum[100];
    MPI_Status status;

    // Define MPI request variables for non-blocking communication
    MPI_Request req_s, req_r;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get process ID (rank) and number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (pid == 0) { // Process 0 generates data
        // Generate data for rows 0 to 49
        for (i = 0; i < 50; i++)
            for (j = 0; j < 100; j++)
                data[i][j] = generate_data(i, j);

        // Set message tag
        mtag = 1;

        // Start non-blocking send to process 1 for data of rows 0 to 49
        MPI_Isend(data, 5000, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);

        // Generate data for rows 50 to 99
        for (i = 50; i < 100; i++)
            for (j = 0; j < 100; j++)
                data[i][j] = generate_data(i, j);

        // Compute row sums for rows 50 to 99
        for (i = 50; i < 100; i++) {
            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];
        }

        // Wait for the non-blocking send to complete
        MPI_Wait(&req_s, &status);

        // Receive computed row sums from process 1
        mtag = 2;
        MPI_Recv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);

        // Printing row sums
        for (i = 0; i < 100; i++) {
            printf(" %d ", row_sum[i]);
            if (i % 5 == 0) printf("\n");
        }
    } else { // Process 1
        // Set message tag for receiving data from process 0
        mtag = 1;

        // Start non-blocking receive for data of rows 0 to 49 from process 0
        MPI_Irecv(data, 5000, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);

        // Wait for the non-blocking receive to complete
        MPI_Wait(&req_r, &status);

        // Compute row sums for rows 0 to 49
        for (i = 0; i < 50; i++) {
            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];
        }

        // Set message tag for sending computed row sums to process 0
        mtag = 2;

        // Send computed row sums of rows 0 to 49 to process 0
        MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
