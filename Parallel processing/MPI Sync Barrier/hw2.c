/* Group-29:
1.	Manvitha Aathmuri – R11847781
2.	Raviteja Sirisanagandla - R11851152
3.	Sai Tejaswini Chirumamilla - R11840694
4.	Preetham Alwala - R11846544
5.	Girish Gandham - R11870388
6.	Ganesh Reddy Mannem - R11849144
*/
#include <stdio.h>
#include <mpi.h>

void mybarrier(MPI_Comm comm) {
    int id, size;

    // MPI Rank Initialize
    MPI_Comm_rank(comm, &id);

    // MPI Size Initialize
    MPI_Comm_size(comm, &size);

    // Initilaizing Log size
    int log2_size = 0;
    int temp = 1;

    // Calculating log2(size)
    while (temp < size) {
        temp <<= 1;
        log2_size++;
    }

    // MPI calls
    int mask = 1;
    for (int step = 0; step < log2_size; step++) {
        int partner = id ^ mask;
        if (id < size && partner < size) {
            MPI_Send(NULL, 0, MPI_INT, partner, 0, comm);
            MPI_Recv(NULL, 0, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
        }
        mask <<= 1;
    }
}

int main(int argc, char** argv) {

    // MPI Initialize
    MPI_Init(&argc, &argv);

    // Comm Initialize
    MPI_Comm comm = MPI_COMM_WORLD;
    int id;

    // Comm rank Initialize
    MPI_Comm_rank(comm, &id);

    // Perform some computation before the barrier
    printf("Process %d is doing some computation.\n", id);

    // Call barrier synchronization
    mybarrier(comm);

    // Perform some computation after the barrier
    printf("Process %d completed the barrier and continues.\n", id);

    // MPI Finalize
    MPI_Finalize();
    return 0;
}
