//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
#include <unistd.h>
#endif  //_DEBUG

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

#ifdef _DEBUG
  printf("Node %i waiting 10 seconds for debugger attachment.\n", node_id);
  sleep(10);
#endif  // debug

  printf("Node %i starting.\n", node_id);

  if (node_id == 0) {
    assert(argc > 1);

    int nodes;
    err = MPI_Comm_size(MPI_COMM_WORLD, &nodes);
    nodes--;  // Not counting node 0, us!

    printf("Reading input dataset.\n");
    int input_size = 0;
    double complex* data = csv2cmplx(argv[1], &input_size);
    assert(data != NULL);

    printf("Calculating node partitions.\n");
    int parts[nodes];
    partition_pow2(input_size, parts, nodes);

    printf("Building communication tree.\n");
    int result_size[nodes];
    int result_dest[nodes];
    result_targets(result_size, result_dest, parts, nodes);

    printf("Applying bit reversal permutation to input dataset.\n");
    bit_reversal_permutation(data, input_size);

    // TODO Look into MPI_Scatterv, it looks like it can do this automatically
    int data_start = 0;
    for (int node = 1; node <= nodes; node++) {
      printf("Sending initial header to node %i.\n", node);
      int index = node - 1;
      err = MPI_Send(
          (int[]){parts[index], result_size[index], result_dest[index]},
          HEADER_SIZE, MPI_INT, node, SEND_HEADER_TAG, MPI_COMM_WORLD);

      printf("Sending subset of size %i to node %i.\n", parts[index], node);
      err = MPI_Send(&data[data_start], parts[index], MPI_DOUBLE_COMPLEX,
                     node, SEND_SUBSET_TAG, MPI_COMM_WORLD);

      data_start += parts[index];
    }

    MPI_Status status;
    MPI_Recv(data, input_size, MPI_DOUBLE_COMPLEX, nodes, SEND_RESULT_TAG,
             MPI_COMM_WORLD, &status);
    int received = 0;
    MPI_Get_count(&status, MPI_DOUBLE_COMPLEX, &received);
    assert(received == input_size);

    print_complex(data, input_size);

    free(data);
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

    double complex data[header[RESULT_SIZE]];
    int data_start = header[RESULT_SIZE] - header[SUBSET_SIZE];
    int data_size = header[SUBSET_SIZE];
    err = MPI_Recv(&data[data_start], header[SUBSET_SIZE], MPI_DOUBLE_COMPLEX, 0,
                   SEND_SUBSET_TAG, MPI_COMM_WORLD, &status);
    printf("Node %i received subset of size %i.\n", node_id,
           header[SUBSET_SIZE]);

    // perform
    fft(&data[data_start], header[SUBSET_SIZE]);

    // TODO More clever use of flow control could reduce redundant operations
    fft_buffer buf = fft_buffer_init();
    while (data_size < header[RESULT_SIZE]) {
      // The upper bounds on size should be this / 2, the extra space is
      // just-in-case
      double complex temp[header[RESULT_SIZE]];

      err = MPI_Recv(temp, header[RESULT_SIZE], MPI_DOUBLE_COMPLEX,
                     MPI_ANY_SOURCE, SEND_RESULT_TAG, MPI_COMM_WORLD, &status);
      int size_received = 0;
      err = MPI_Get_count(&status, MPI_DOUBLE_COMPLEX, &size_received);
      printf("Node %i received result of size %i.\n", node_id, size_received);

      fft_buffer_add(buf, temp, size_received);
      double complex* match;
      while ((match = fft_buffer_search(buf, data_size)) != NULL) {
        data_start -= data_size;
        memcpy(&data[data_start], match, sizeof(double complex) * data_size);
        data_size *= 2;
        fft_pass(&data[data_start], data_size, data_size);
      }
    }
    fft_buffer_free(&buf);

    printf("Node %i sending result of size %i to node %i.\n", node_id,
           header[RESULT_SIZE], header[RESULT_DEST]);
    MPI_Send(data, header[RESULT_SIZE], MPI_DOUBLE_COMPLEX, header[RESULT_DEST],
             SEND_RESULT_TAG, MPI_COMM_WORLD);
  }

  printf("Node %i finished.\n", node_id);
  err = MPI_Finalize();

  return 0;
}
