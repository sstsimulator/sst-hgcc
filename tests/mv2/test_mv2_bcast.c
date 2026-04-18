/* 8-rank MPI_Bcast correctness test. Root 0 broadcasts buf[i]=i; all verify. */
#define ssthg_app_name test_mv2_bcast
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_SIZE 8
#define N 16

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank = -1, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != EXPECTED_SIZE) {
    if (rank == 0) {
      fprintf(stderr, "test_mv2_bcast: need size == %d (got %d)\n", EXPECTED_SIZE, size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int buf[N];
  if (rank == 0) {
    for (int i = 0; i < N; i++) buf[i] = i;
  } else {
    for (int i = 0; i < N; i++) buf[i] = -1;
  }

  int rc = MPI_Bcast(buf, N, MPI_INT, 0, MPI_COMM_WORLD);
  if (rc != MPI_SUCCESS) {
    fprintf(stderr, "test_mv2_bcast: rank %d MPI_Bcast rc=%d\n", rank, rc);
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  for (int i = 0; i < N; i++) {
    if (buf[i] != i) {
      fprintf(stderr, "test_mv2_bcast: rank %d buf[%d]=%d expected %d\n",
              rank, i, buf[i], i);
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
  }

  if (rank == 0) {
    printf("PASS: test_mv2_bcast (8 ranks)\n");
  }

  MPI_Finalize();
  return 0;
}
