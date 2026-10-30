// Exercise 5.2 - message2.cc rewritten using Buffered Send (MPI_Bsend).
// A separate array element is used for every message so the data being
// sent is never overwritten by the next message.
// Compile: mpicxx message2_bsend.cc -o message2_bsend
// Run    : mpirun -np 2 ./message2_bsend
#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int NMSG = 3;
    int sendbuf[NMSG];   // one variable per message sent
    int recvbuf[NMSG];   // one variable per message received

    if (rank == 0) {
        // Buffer big enough for all NMSG buffered messages
        int msgsize = 0;
        MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &msgsize);
        int bufsize = NMSG * (msgsize + MPI_BSEND_OVERHEAD);
        char* buffer = (char*)malloc(bufsize);
        MPI_Buffer_attach(buffer, bufsize);

        for (int i = 0; i < NMSG; i++) {
            sendbuf[i] = i * 10;
            MPI_Bsend(&sendbuf[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 sent " << sendbuf[i] << std::endl;
        }

        // Waits until all buffered messages have been transmitted
        MPI_Buffer_detach(&buffer, &bufsize);
        free(buffer);
    } else if (rank == 1) {
        for (int i = 0; i < NMSG; i++) {
            MPI_Recv(&recvbuf[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << recvbuf[i] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
