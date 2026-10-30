// Exercise 3 - Monte Carlo computation of Pi using MPI
// Based on the serial Dartmouth example: throw random points into the unit
// square and count how many land inside the quarter circle.
// Compile: mpicxx -O2 pi_mc.cc -o pi_mc
// Run    : mpirun -np 4 ./pi_mc          (optional arg: iterations, default 10000000)
#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long niter = (argc > 1) ? atoll(argv[1]) : 10000000LL;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // Split the iterations among processes
    long long my_iter = niter / size + (rank < niter % size ? 1 : 0);

    // Each process gets its own random stream
    unsigned int seed = 35791246u + 7919u * (unsigned)rank;

    long long count = 0;
    for (long long i = 0; i < my_iter; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) count++;
    }

    if (rank == 0) {
        long long total = count;
        printf("Received from process %d : count = %lld\n", 0, count);
        // Receive in fixed order: process 1, 2, 3, ...
        for (int src = 1; src < size; src++) {
            long long part;
            MPI_Status status;
            MPI_Recv(&part, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, &status);
            printf("Received from process %d : count = %lld\n", status.MPI_SOURCE, part);
            total += part;
        }
        double pi = 4.0 * (double)total / (double)niter;
        double t1 = MPI_Wtime();
        printf("Processes      : %d\n", size);
        printf("Iterations     : %lld\n", niter);
        printf("Pi estimate    : %.10f\n", pi);
        printf("Error          : %.10f\n", fabs(pi - M_PI));
        printf("Time (seconds) : %.6f\n", t1 - t0);
    } else {
        MPI_Send(&count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
