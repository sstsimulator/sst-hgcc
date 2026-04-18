/* 8-rank MPI_Reduce correctness test. Each rank contributes (rank+1); SUM on root 0. */
#define ssthg_app_name test_mv2_reduce
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
      fprintf(stderr, "test_mv2_reduce: need size == %d (got %d)\n", EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int in = rank + 1;
  int out = -1;
  int rc = MPI_Reduce(&in, &out, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_reduce: rank %d MPI_Reduce rc=%d\n", rank, rc);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  if (rank == 0) {
    int expected = size * (size + 1) / 2;  /* 36 for size=8 */
    if (out != expected) {
      fprintf(stderr, "test_mv2_reduce: root got %d expected %d\n", out, expected);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
    printf("PASS: test_mv2_reduce (8 ranks, SUM=%d)\n", out);
  }

  MPI_Finalize();
  return 0;
}
