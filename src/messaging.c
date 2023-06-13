//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include "messaging.h"

#include <mpi.h>

#include "logging.h"

#define SEND_HEADER_TAG 5260
#define SEND_SUBSET_TAG 5261
#define SEND_RESULT_TAG 5262

#define HEADER_SIZE 3
#define SUBSET_SIZE 0
#define RESULT_SIZE 1
#define RESULT_DEST 2

int msg_init(int *argc, char **argv[]) {
  MPI_Init(argc, argv);
  int node_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &node_id);
  return node_id;
}

int get_node_count() {
  int nodes;
  MPI_Comm_size(MPI_COMM_WORLD, &nodes);
  return nodes;
}

void send_headers(int parts[], int result_size[], int result_dest[],
                  int nodes) {
  for (int node = 1; node <= nodes; node++) {
    log_msg(LOG__INFO, "Sending initial header to node %i.", node);
    log_msg(LOG_DEBUG, "Header contents: {%i, %i, %i}", parts[node - 1],
            result_size[node - 1], result_dest[node - 1]);
    MPI_Send(
        (int[]){parts[node - 1], result_size[node - 1], result_dest[node - 1]},
        HEADER_SIZE, MPI_INT, node, SEND_HEADER_TAG, MPI_COMM_WORLD);
  }
}

void recv_header(int *subset_size, int *result_size, int *result_dest) {
  MPI_Status status;
  int header[HEADER_SIZE];
  MPI_Recv(&header, HEADER_SIZE, MPI_INT, 0, SEND_HEADER_TAG, MPI_COMM_WORLD,
           &status);
  log_msg(LOG__INFO, "Inital header received.");
  log_msg(LOG_DEBUG, "Header contents: {%i, %i, %i}", header[SUBSET_SIZE],
          header[RESULT_SIZE], header[RESULT_DEST]);
  (*subset_size) = header[SUBSET_SIZE];
  (*result_size) = header[RESULT_SIZE];
  (*result_dest) = header[RESULT_DEST];
}

void send_init_subsets(double complex data[], int parts[], int nodes) {
  // TODO Look into MPI_Scatterv, it looks like it can do this automatically
  int data_start = 0;
  for (int node = 1; node <= nodes; node++) {
    if (parts[node - 1] == 0) continue;  // Skip sending to empty nodes.
    log_msg(LOG__INFO, "Sending subset of size %i to node %i.", parts[node - 1],
            node);
    MPI_Send(&data[data_start], parts[node - 1], MPI_DOUBLE_COMPLEX, node,
             SEND_SUBSET_TAG, MPI_COMM_WORLD);

    data_start += parts[node - 1];
  }
}

int recv_init_subset(double complex *data, int max) {
  MPI_Status status;
  MPI_Recv(data, max, MPI_DOUBLE_COMPLEX, 0, SEND_SUBSET_TAG, MPI_COMM_WORLD,
           &status);
  int received = 0;
  MPI_Get_count(&status, MPI_DOUBLE_COMPLEX, &received);
  log_msg(LOG__INFO, "Initial subset of size %i received.", received);
  return received;
}

void send_results(double complex *data, int size, int dest) {
  log_msg(LOG__INFO, "Sending result of size %i to node %i.", size, dest);
  MPI_Send(data, size, MPI_DOUBLE_COMPLEX, dest, SEND_RESULT_TAG,
           MPI_COMM_WORLD);
}

int recv_result_set(double complex *data, int max) {
  MPI_Status status;
  MPI_Recv(data, max, MPI_DOUBLE_COMPLEX, MPI_ANY_SOURCE, SEND_RESULT_TAG,
           MPI_COMM_WORLD, &status);
  int received = 0;
  MPI_Get_count(&status, MPI_DOUBLE_COMPLEX, &received);
  log_msg(LOG__INFO, "Received result of size %i.", received);
  return received;
}

void msg_finalize() {
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}

void msg_abort() { MPI_Abort(MPI_COMM_WORLD, 1); }