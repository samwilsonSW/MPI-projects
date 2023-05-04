#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/*
Input: n, x[n]. Note that x[i] != x[j] for i != j.
Output: f[n].
 */

#define sgn(x)  ( ((x)<0.0) ? (-1.0) : (1.0) )
#define c1      1.23456
#define c2      6.54321

void calc_force(int n, double *x, double *f) {
    int i, j;

    double diff, tmp;
    for (i=0; i<n; i++){ f[i] = 0.0;}
        
    for (i = 1; i < n; i++) {
        for (j = 0; j<i; j++) {
            diff = x[i] - x[j];
            tmp = c1/(diff*diff*diff) - c2*sgn(diff)/(diff*diff);
            f[i] += tmp;
            f[j] -= tmp;
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size, i, j, n = 100;
    double *x, *f, *f_local, *x_local;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // allocate memory for x and f arrays
    x = (double *)malloc(n * sizeof(double));
    f = (double *)calloc(n, sizeof(double));
    f_local = (double *)calloc(n/size, sizeof(double));
    x_local = (double *)malloc((n/size) * sizeof(double));

    // initialize x array
    for (i = 0; i < n; i++) {
        x[i] = i * 0.001;
    }

    // scatter x array
    MPI_Scatter(x, n/size, MPI_DOUBLE, x_local, n/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // calculate forces
    calc_force(n/size, x_local, f_local);

    // gather forces
    MPI_Gather(f_local, n/size, MPI_DOUBLE, f, n/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // print forces
    if (rank == 0) {
        for (i = 0; i < n; i++) {
            printf("F[%d] = %f\n", i+1, f[i]);
        }
    }

    // free memory
    free(x);
    free(f);
    free(f_local);
    free(x_local);

    MPI_Finalize();
    return 0;
}
