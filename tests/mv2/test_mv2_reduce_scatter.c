/* 8-rank MPI_Reduce_scatter. Each rank contributes {1,1,...,1} of len N*size; SUM;
 * each rank receives 2 ints; both must equal size. */
#define ssthg_app_name test_mv2_reduce_scatter
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_SIZE 8
#define PER_RANK 2

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank = -1, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != EXPECTED_SIZE) {
    if (rank == 0) {
      fprintf(stderr, "test_mv2_reduce_scatter: need size == %d (got %d)\n",
              EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int total = PER_RANK * size;
  int* send = (int*)malloc(sizeof(int) * total);
  int recv[PER_RANK];
  int* rcounts = (int*)malloc(sizeof(int) * size);

  for (int i = 0; i < total; i++) send[i] = 1;
  for (int i = 0; i < PER_RANK; i++) recv[i] = -1;
  for (int i = 0; i < size; i++) rcounts[i] = PER_RANK;

  int rc = MPI_Reduce_scatter(send, recv, rcounts, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_reduce_scatter: rank %d rc=%d\n", rank, rc);
    free(send); free(rcounts);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  for (int i = 0; i < PER_RANK; i++) {
    if (recv[i] != size) {
      fprintf(stderr, "test_mv2_reduce_scatter: rank %d recv[%d]=%d expected %d\n",
              rank, i, recv[i], size);
      free(send); free(rcounts);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
  }
  free(send); free(rcounts);

  if (rank == 0) {
    printf("PASS: test_mv2_reduce_scatter (8 ranks)\n");
  }

  MPI_Finalize();
  return 0;
}
