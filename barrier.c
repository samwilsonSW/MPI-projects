#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, size, i, p;
    MPI_Init(&argc, &argv);   // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // Get rank from process ID
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Get the total number of processes

    // Calculate the number of steps required to achieve synchronization
    int limit = 2 * ceil(log2(size));

    // Perform the first half of the synchronization
    for (p = 0; p < limit; p++) {
        int j;
        for (j = 0; j < size; j++) {
            if (j != rank) {
                int pow2p = pow(2, p/2);
                int partner = (rank + pow2p) % size;
                if (p % 2 == 0) {   // Even step, send message to partner
                    printf("Process %d sends message to process %d\n", rank, partner);
                    MPI_Send(NULL, 0, MPI_INT, partner, 0, MPI_COMM_WORLD);
                } else {   // Odd step, receive message from partner
                    printf("Process %d receives message from process %d\n", rank, partner);
                    MPI_Recv(NULL, 0, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
        }
    }

    // Perform the second half of the synchronization, flip recv and send for barrier synchronization
    for (p = 0; p < limit; p++) {
        int j;
        for (j = 0; j < size; j++) {
            if (j != rank) {
                int pow2p = pow(2, p/2);
                int partner = (rank + pow2p) % size;
                if (p % 2 == 0) {   // Even step, receive message from partner
                    MPI_Recv(NULL, 0, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                } else {   // Odd step, send message to partner
                    MPI_Send(NULL, 0, MPI_INT, partner, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // Print message once all processes have completed the synchronization
    printf("Process %d has completed the barrier synchronization\n", rank);

    MPI_Finalize();   // Finalize MPI
    return 0;
}
