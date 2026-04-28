/* 8-rank MPI_Scatter test. Root 0 scatters [0..N*K-1]; each rank verifies its chunk. */
#define ssthg_app_name test_mv2_scatter
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_SIZE 8
#define K 4

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank = -1, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != EXPECTED_SIZE) {
    if (rank == 0) {
      fprintf(stderr, "test_mv2_scatter: need size == %d (got %d)\n", EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int* send = NULL;
  if (rank == 0) {
    send = (int*)malloc(sizeof(int) * K * size);
    for (int i = 0; i < K * size; i++) send[i] = i;
  }

  int recv[K];
  for (int i = 0; i < K; i++) recv[i] = -1;

  int rc = MPI_Scatter(send, K, MPI_INT, recv, K, MPI_INT, 0, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_scatter: rank %d rc=%d\n", rank, rc);
    if (rank == 0) free(send);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  for (int i = 0; i < K; i++) {
    int expected = rank * K + i;
    if (recv[i] != expected) {
      fprintf(stderr, "test_mv2_scatter: rank %d recv[%d]=%d expected %d\n",
              rank, i, recv[i], expected);
      if (rank == 0) free(send);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
  }

  if (rank == 0) {
    free(send);
    printf("PASS: test_mv2_scatter (8 ranks)\n");
  }

  MPI_Finalize();
  return 0;
}
