// Exercise 2 - Parallel sum of 1 .. 10,000,000 using MPI
// Compile: mpicxx -O2 sum.cc -o sum
// Run    : mpirun -np 4 ./sum            (optional arg: N, default 10000000)
#include <mpi.h>
#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = (argc > 1) ? atoll(argv[1]) : 10000000LL;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // Block decomposition of the range 1..N among the processes
    long long chunk = N / size;
    long long rem   = N % size;
    long long start = rank * chunk + (rank < rem ? rank : rem) + 1;
    long long end   = start + chunk - 1 + (rank < rem ? 1 : 0);

    long long local_sum = 0;
    for (long long i = start; i <= end; i++)
        local_sum += i;

    long long total = 0;
    if (rank == 0) {
        total = local_sum;
        for (int src = 1; src < size; src++) {
            long long part;
            MPI_Recv(&part, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += part;
        }
    } else {
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    double t1 = MPI_Wtime();

    printf("Process %d summed %lld .. %lld = %lld\n", rank, start, end, local_sum);
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Processes      : %d\n", size);
        printf("Total sum      : %lld\n", total);
        printf("Expected       : %lld\n", N * (N + 1) / 2);
        printf("Time (seconds) : %.6f\n", t1 - t0);
    }

    MPI_Finalize();
    return 0;
}
