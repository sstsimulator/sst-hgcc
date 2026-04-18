/* 8-rank MPI_Allreduce correctness test. Each rank contributes (rank+1); SUM to all. */
#define ssthg_app_name test_mv2_allreduce
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
      fprintf(stderr, "test_mv2_allreduce: need size == %d (got %d)\n", EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int in = rank + 1;
  int out = -1;
  int rc = MPI_Allreduce(&in, &out, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_allreduce: rank %d MPI_Allreduce rc=%d\n", rank, rc);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  int expected = size * (size + 1) / 2;  /* 36 for size=8 */
  if (out != expected) {
    fprintf(stderr, "test_mv2_allreduce: rank %d got %d expected %d\n",
            rank, out, expected);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  if (rank == 0) {
    printf("PASS: test_mv2_allreduce (8 ranks, SUM=%d)\n", out);
  }

  MPI_Finalize();
  return 0;
}
