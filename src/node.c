//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include "node.h"

#include <assert.h>
#include <complex.h>
#include <stdlib.h>
#include <string.h>

#include "fft.h"
#include "logging.h"
#include "messaging.h"

void head_node(const char* filename) {
  int nodes = get_node_count();
  nodes--;  // Not counting node 0, us!

  log_msg(LOG__INFO, "Reading input dataset.");
  int input_size = 0;
  double complex* data = csv2cmplx(filename, &input_size);
  if (data == NULL) {
    log_msg(LOG_FATAL, "Unable to read input file: %s", filename);
    // TODO Add MPI Abort call
  }

  log_msg(LOG__INFO, "Calculating node partitions.");
  int parts[nodes];
  partition_pow2(input_size, parts, nodes);

  log_msg(LOG__INFO, "Building communication tree.");
  int result_size[nodes];
  int result_dest[nodes];
  result_targets(result_size, result_dest, parts, nodes);

  log_msg(LOG__INFO, "Applying bit reversal permutation to input dataset.");
  bit_reversal_permutation(data, input_size);

  send_headers(parts, result_size, result_dest, nodes);

  send_init_subsets(data, parts, nodes);

  recv_result_set(data, input_size);

  print_complex(data, input_size);

  free(data);
}

void data_node(int node_id) {
  int subset_size, result_size, result_dest;

  recv_header(&subset_size, &result_size, &result_dest);
  log_msg(LOG__INFO, "Received header.");

  if (subset_size == 0) {
    log_msg(LOG__INFO, "Received subset size of 0, terminating.");
    return;
  }

  double complex data[result_size];
  int data_start = result_size - subset_size;
  int data_size = subset_size;
  recv_init_subset(&data[data_start], subset_size);
  log_msg(LOG__INFO,"Received subset of size %i.", subset_size);

  // perform
  log_msg(LOG_DEBUG, "Starting inital FFT calculation.");
  fft(&data[data_start], subset_size);
  log_msg(LOG_DEBUG, "Finished inital FFT calculation.");

  // TODO More clever use of flow control could reduce redundant operations
  fft_buffer buf = fft_buffer_init();
  while (data_size < result_size) {
    // The upper bounds on size should actually be this / 2, the extra space
    // is just-in-case
    double complex temp[result_size];
    int size_received = recv_result_set(temp, result_size);

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
  send_results(data, result_size, result_dest);
}