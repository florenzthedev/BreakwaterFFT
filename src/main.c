//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO Add compatibility stuff for VSC++
#ifdef _DEBUG
#include <unistd.h>
#endif  //_DEBUG

#include "fft.h"
#include "messaging.h"

int main(int argc, char* argv[]) {
  int node_id;

  node_id = msg_init(&argc, &argv);

#ifdef _DEBUG
  printf("Node %i waiting 10 seconds for debugger attachment.\n", node_id);
  sleep(10);
#endif  // debug

  printf("Node %i starting.\n", node_id);

  if (node_id == 0) {
    assert(argc > 1);

    int nodes = get_node_count();
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

    send_headers(parts, result_size, result_dest, nodes);

    send_init_subsets(data, parts, nodes);

    // TODO get final result here.

    print_complex(data, input_size);

    free(data);
  } else {
    int subset_size, result_size, result_dest;

    recv_header(&subset_size, &result_size, &result_dest);
    printf("Node %i received header.\n", node_id);

    if (subset_size == 0) {
      printf("Node %i received subset size of 0, terminating.\n", node_id);
      msg_finalize();
      return 0;
    }

    double complex data[result_size];
    int data_start = result_size - subset_size;
    int data_size = subset_size;
    recv_init_subset(&data[data_start], subset_size);
    printf("Node %i received subset of size %i.\n", node_id, subset_size);

    // perform
    fft(&data[data_start], subset_size);

    // TODO More clever use of flow control could reduce redundant operations
    fft_buffer buf = fft_buffer_init();
    while (data_size < result_size) {
      // The upper bounds on size should be this / 2, the extra space is
      // just-in-case
      double complex temp[result_size];

      MPI_Recv(temp, header[RESULT_SIZE], MPI_DOUBLE_COMPLEX, MPI_ANY_SOURCE,
               SEND_RESULT_TAG, MPI_COMM_WORLD, &status);
      int size_received = 0;
      MPI_Get_count(&status, MPI_DOUBLE_COMPLEX, &size_received);
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
           result_size, result_dest);
    MPI_Send(data, header[RESULT_SIZE], MPI_DOUBLE_COMPLEX, header[RESULT_DEST],
             SEND_RESULT_TAG, MPI_COMM_WORLD);
  }

  printf("Node %i finished.\n", node_id);
  msg_finalize();
  return 0;
}
