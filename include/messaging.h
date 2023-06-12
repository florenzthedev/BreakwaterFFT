//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef MESSAGING_H_INCLUDED
#define MESSAGING_H_INCLUDED

#include <complex.h>

/**
 * @brief Wrapper around MPI_Init and MPI_Comm_rank. Here so mpi.h does not need
 * to be included in main. Arguments are just passed in from main.
 *
 * @param argc Copy of argc from main, will be modified to be as though the
 * program was invoked normally instead of with mpirun.
 * @param argv Copy of argc from main, will be modified to be as though the
 * program was invoked normally instead of with mpirun.
 * @return int The rank of the current node.
 */
int msg_init(int *argc, char **argv[]);

/**
 * @brief Get the number of nodes in the current system.
 *
 * @return Number of nodes.
 */
int get_node_count();

/**
 * @brief Packages and sends the initial headers to all other nodes in the
 * system. The input arrays are expected to be parallel and each index
 * corresponds to the (index + 1)'th node.
 *
 * @param parts The size of the subset that will be sent to each respective
 * node.
 * @param result_size The size of the result each respective node is expected to
 * send.
 * @param result_dest The destination node for the result from each node.
 * @param nodes The total number of nodes, not counting the head node.
 */
void send_headers(int parts[], int result_size[], int result_dest[], int nodes);

/**
 * @brief
 *
 * @param subset_size
 * @param result_size
 * @param result_dest
 */
void recv_header(int *subset_size, int *result_size, int *result_dest);

/**
 * @brief Sends the initial subsets
 *
 * @param data
 * @param parts
 * @param nodes
 */
void send_init_subsets(double complex data[], int parts[], int nodes);

/**
 * @brief 
 * 
 * @param data 
 * @param max 
 * @return int 
 */
int recv_init_subset(double complex *data, int max);

/**
 * @brief
 *
 * @param data
 * @param max
 * @return int
 */
int recv_result_set(double complex *data, int max);

/**
 * @brief Stub function calling MPI_Finalize().
 *
 */
void msg_finalize();

#endif  // MESSAGING_H_INCLUDED