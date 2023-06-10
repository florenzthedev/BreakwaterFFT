//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "fft.h"

int main(int argc, char* argv[]) {
  int err, node_id, num_nodes;

  err = MPI_Init(&argc, &argv);
  err = MPI_Comm_rank(MPI_COMM_WORLD, &node_id);
  err = MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

  if (node_id == 0) {
    assert(argc > 1);
    int N = 0;
    double complex* x = csv2cmplx(argv[1], &N);
    assert(x != NULL);
    for (int i = 0; i < N; i++) printf("%f + %fi, ", creal(x[i]), cimag(x[i]));
    printf("\n");

    int parts[N];
    partition_fft(N, parts, num_nodes);
    bit_reversal_permutation(x, N);

    for (int i = 0; i < num_nodes; i++) printf("%i, ", parts[i]);
    printf("\n");
    for (int i = 0; i < N; i++) printf("%f + %fi, ", creal(x[i]), cimag(x[i]));
    printf("\n");

    free(x);
  }

  printf("Hello, world!\n");

  err = MPI_Finalize();

  return 0;
}
