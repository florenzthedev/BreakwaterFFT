//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "fft.h"

#define SEND_HEADER_TAG 5260
#define SEND_SUBSET_TAG 5261
#define SEND_RESULT_TAG 5262

#define HEADER_SIZE 3
#define SUBSET_SIZE 0
#define RESULT_SIZE 1
#define RESULT_DEST 2

int main(int argc, char* argv[]) {
  int err, node_id;

  err = MPI_Init(&argc, &argv);
  err = MPI_Comm_rank(MPI_COMM_WORLD, &node_id);

  printf("Node %i starting.\n", node_id);

  if (node_id == 0) {
    assert(argc > 1);

    int nodes;
    err = MPI_Comm_size(MPI_COMM_WORLD, &nodes);
    nodes--;  // Not counting node 0, us!

    printf("Reading input dataset.\n");
    int N = 0;
    double complex* x = csv2cmplx(argv[1], &N);
    assert(x != NULL);

    printf("Calculating node partitions.\n");
    int parts[N];
    partition_pow2(N, parts, nodes);

    printf("Building communication tree.\n");
    int result_size[N];
    int result_dest[N];
    result_targets(result_size, result_dest, parts, nodes);

    printf("Applying bit reversal permutation to input dataset.\n");
    bit_reversal_permutation(x, N);

    // TODO Look into MPI_Scatterv, it looks like it can do this automatically
    int data_start = 0;
    for (int node = 1; node <= nodes; node++) {
      printf("Sending initial header to node %i.\n", node);
      int index = node - 1;
      err = MPI_Send(
          (int[]){parts[index], result_size[index], result_dest[index]},
          HEADER_SIZE, MPI_INT, node, SEND_HEADER_TAG, MPI_COMM_WORLD);

      printf("Sending subset of size %i to node %i.\n", parts[index], node);
      err = MPI_Send(&x[data_start], parts[index], MPI_DOUBLE_COMPLEX, node,
                     SEND_SUBSET_TAG, MPI_COMM_WORLD);

      data_start += parts[index];
    }

    free(x);
  } else {
    MPI_Status status;
    int header[HEADER_SIZE];
    err = MPI_Recv(&header, HEADER_SIZE, MPI_INT, 0, SEND_HEADER_TAG,
                   MPI_COMM_WORLD, &status);
    assert(err == MPI_SUCCESS);
    printf("Node %i received header.\n", node_id);

    if (header[SUBSET_SIZE] == 0) {
      printf("Node %i received subset size of 0, terminating.\n", node_id);
      err = MPI_Finalize();
      return 0;
    }

    double complex x[header[RESULT_SIZE]];
    int data_start = header[RESULT_SIZE] - header[SUBSET_SIZE];
    int data_size = header[SUBSET_SIZE];
    err = MPI_Recv(&x[data_start], header[SUBSET_SIZE], MPI_DOUBLE_COMPLEX, 0,
                   SEND_SUBSET_TAG, MPI_COMM_WORLD, &status);
    printf("Node %i received subset of size %i.\n", node_id,
           header[SUBSET_SIZE]);

    //perform 
    fft(&x[data_start], header[SUBSET_SIZE]);

    while (data_start > 0) {
      double complex temp[header[RESULT_SIZE]];
      err = MPI_Recv(temp, header[RESULT_SIZE], MPI_DOUBLE_COMPLEX,
                     MPI_ANY_SOURCE, SEND_RESULT_TAG, MPI_COMM_WORLD, &status);
      int size_received = 0;
      err = MPI_Get_count(&status, MPI_DOUBLE_COMPLEX, &size_received);
      
      
    }
  }

  printf("Node %i finished.\n", node_id);
  err = MPI_Finalize();

  return 0;
}
