/* 8-rank MPI_Barrier correctness test under SST (hgcc globals + PMI). */
#define ssthg_app_name test_mv2_barrier
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_SIZE 8

int main(int argc, char** argv) {
  fprintf(stderr, "[test_mv2_barrier] entering MPI_Init\n");
  MPI_Init(&argc, &argv);
  int rank = -1, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  fprintf(stderr, "[test_mv2_barrier] rank=%d size=%d\n", rank, size);

  if (size != EXPECTED_SIZE) {
    if (rank == 0) {
      fprintf(stderr, "test_mv2_barrier: need MPI_COMM_WORLD size == %d (got %d)\n",
              EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  /* Two back-to-back barriers; any protocol error or rank drop-out will either
   * deadlock (timing out the simulator) or return non-MPI_SUCCESS. */
  int rc = MPI_Barrier(MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_barrier: rank %d MPI_Barrier #1 returned %d\n", rank, rc);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }
  rc = MPI_Barrier(MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_barrier: rank %d MPI_Barrier #2 returned %d\n", rank, rc);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  if (rank == 0) {
    printf("PASS: test_mv2_barrier (8 ranks)\n");
  }

  MPI_Finalize();
  return 0;
}
