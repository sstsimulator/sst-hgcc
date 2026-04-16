/* Two-rank MPI send/recv under SST (hgcc globals + PMI). */
#define ssthg_app_name test_mv2_sendrecv
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  fprintf(stderr, "[test_mv2] entering MPI_Init\n");
  MPI_Init(&argc, &argv);
  fprintf(stderr, "[test_mv2] MPI_Init done\n");
  int rank = -1, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  fprintf(stderr, "[test_mv2] rank=%d size=%d\n", rank, size);

  if (size < 2) {
    if (rank == 0) {
      fprintf(stderr, "test_mv2_sendrecv: need MPI_COMM_WORLD size >= 2 (got %d)\n",
              size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int payload = -1;
  if (rank == 0) {
    payload = 42;
    fprintf(stderr, "[test_mv2] rank 0 entering MPI_Send\n");
    MPI_Send(&payload, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    fprintf(stderr, "[test_mv2] rank 0 MPI_Send done\n");
  } else if (rank == 1) {
    fprintf(stderr, "[test_mv2] rank 1 entering MPI_Recv\n");
    MPI_Recv(&payload, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    fprintf(stderr, "[test_mv2] rank 1 MPI_Recv done\n");
    if (payload != 42) {
      fprintf(stderr, "test_mv2_sendrecv: rank 1 expected 42, got %d\n", payload);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
  }

  if (rank == 0) {
    printf("PASS: 2-rank MPI_Send/MPI_Recv under SST\n");
  }

  fprintf(stderr, "[test_mv2] rank %d entering MPI_Finalize\n", rank);
  MPI_Finalize();
  fprintf(stderr, "[test_mv2] rank %d MPI_Finalize done\n", rank);
  return 0;
}
