/* 8-rank MPI_Allgather test. Each rank sends {rank*K..rank*K+K-1}; all verify. */
#define ssthg_app_name test_mv2_allgather
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
      fprintf(stderr, "test_mv2_allgather: need size == %d (got %d)\n", EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int send[K];
  for (int i = 0; i < K; i++) send[i] = rank * K + i;

  int* recv = (int*)malloc(sizeof(int) * K * size);
  for (int i = 0; i < K * size; i++) recv[i] = -1;

  int rc = MPI_Allgather(send, K, MPI_INT, recv, K, MPI_INT, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_allgather: rank %d rc=%d\n", rank, rc);
    free(recv);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  for (int i = 0; i < K * size; i++) {
    if (recv[i] != i) {
      fprintf(stderr, "test_mv2_allgather: rank %d recv[%d]=%d expected %d\n",
              rank, i, recv[i], i);
      free(recv);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
  }
  free(recv);

  if (rank == 0) {
    printf("PASS: test_mv2_allgather (8 ranks)\n");
  }

  MPI_Finalize();
  return 0;
}
