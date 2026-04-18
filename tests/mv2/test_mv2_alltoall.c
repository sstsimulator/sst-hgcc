/* 8-rank MPI_Alltoall test. sendbuf[j] = rank*100 + j; after: recvbuf[i] = i*100 + rank. */
#define ssthg_app_name test_mv2_alltoall
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_SIZE 8

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank = -1, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != EXPECTED_SIZE) {
    if (rank == 0) {
      fprintf(stderr, "test_mv2_alltoall: need size == %d (got %d)\n", EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int* send = (int*)malloc(sizeof(int) * size);
  int* recv = (int*)malloc(sizeof(int) * size);
  for (int j = 0; j < size; j++) {
    send[j] = rank * 100 + j;
    recv[j] = -1;
  }

  int rc = MPI_Alltoall(send, 1, MPI_INT, recv, 1, MPI_INT, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_alltoall: rank %d rc=%d\n", rank, rc);
    free(send); free(recv);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  for (int i = 0; i < size; i++) {
    int expected = i * 100 + rank;
    if (recv[i] != expected) {
      fprintf(stderr, "test_mv2_alltoall: rank %d recv[%d]=%d expected %d\n",
              rank, i, recv[i], expected);
      free(send); free(recv);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
  }
  free(send); free(recv);

  if (rank == 0) {
    printf("PASS: test_mv2_alltoall (8 ranks)\n");
  }

  MPI_Finalize();
  return 0;
}
