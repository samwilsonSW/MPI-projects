//
//  p&c1.c
//  
//
//  Created by Sam Wilson on 2/6/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define generate_data(i, j) (i)+(j)*(1)
void main(int argc, char **argv) {
    int i, j, pid, np, mtag, count, data[100][100], row_sum [100];
    double to, t1;
    MPI_Status status;
    MPI_Request req_s, req_r;
    MPI_Init( &argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    if (pid == 0) { // generate data[]
        for (i=0; i<50; i++) {
            for(j=0; j<100; j++) {
                data[i][j] = generate_data(i,j);
            }
        }
        mtag = 1;
        MPI_Isend(data, 5000, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);
        
        for (i=50; i<100; i++) {
            for(j=0; j<100; j++) {
                data[i][j] = generate_data(i,j);
            }
        }
        
        for (i=50; i<100; i++) {
            row_sum[i] = 0;
            for(j=0; j<100; j++) {
                row_sum[i] += data[i][j];
            }
        }
        // Receive computed row sums from pid 1
        mtag = 2;
        MPI_Irecv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_r);
        
        MPI_Wait(&req_s, &status);
        // MPI_Wait(&req_r, &status);
        for (i=0; i<100; i++) {
            printf("%d ", row_sum[i]);
            if (i%10 ==0)
                printf("\n");
        }
    }
            
    else { /*** pid == 1 ***/
        mtag= 1;
        MPI_Recv (data, 5000, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
        // Receive computed row sums from pid 0
        for (i=0; i<50; i++) {
            // Begin computation for row sums
            row_sum[i] = 0;
            for(j=0; j<100; j++) {
                row_sum[i] += data[i][j];
            }
        }
        // Send computed row sums to pid 0
        mtag= 2;
        MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD);
    } /****** End of else ******/
    MPI_Finalize();
}
/****** End of main ******/

