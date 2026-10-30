// Exercise 5.1 - message1.cc modified so that the source and destination do not match.
// Rank 0 sends the number to rank 2, but rank 1 is waiting for a message from rank 0
// and rank 2 is waiting for a message from rank 1.
// Compile: mpicxx message1_mismatch.cc -o message1_mismatch
// Run    : mpirun -np 3 ./message1_mismatch
//
// What happens: no receive matches the message, so rank 1 and rank 2 block in
// MPI_Recv forever. Rank 0's small MPI_Send completes (eager/buffered), but the
// program never terminates - it hangs (deadlock) and has to be killed.
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number;
    if (rank == 0) {
        number = 42;
        // Wrong destination: sends to rank 2 instead of rank 1
        MPI_Send(&number, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        std::cout << "Process 0 sent " << number << " to process 2" << std::endl;
    } else if (rank == 1) {
        std::cout << "Process 1 waiting for a message from process 0" << std::endl;
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 1 received " << number << std::endl;
    } else if (rank == 2) {
        // Wrong source: expects the message from rank 1, but it comes from rank 0
        std::cout << "Process 2 waiting for a message from process 1" << std::endl;
        MPI_Recv(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 2 received " << number << std::endl;
    }

    MPI_Finalize();
    return 0;
}
