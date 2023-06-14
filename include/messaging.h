//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

/**
 * @brief This file encapsulates all of the messaging needed by the program.
 * This program was designed with MPI in mind but all of the MPI code was
 * isolated to these functions.
 *
 */
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
 * @brief Receives the initial header from the head node. These variables are
 * enough information for each node to map out what needs to be done.
 *
 * @param subset_size Variable that will store the size of the initial subset
 * from the head node will be.
 * @param result_size Variable that will store the size of the result this node
 * is expected to send.
 * @param result_dest The node that will store the node the result should be
 * sent to.
 */
void recv_header(int *subset_size, int *result_size, int *result_dest);

/**
 * @brief Sends the initial subsets
 *
 * @param data The full set of the bit-reversed permutation'd data to be sent
 * out to other nodes.
 * @param parts The list of sizes to be sent to each node respectively.
 * @param nodes The total number of nodes.
 */
void send_init_subsets(double complex data[], int parts[], int nodes);

/**
 * @brief Receives the inital subset of numbers to perform the FFT on.
 *
 * @param data The buffer to store the incoming data in.
 * @param max The maximum number of elements the incoming data buffer can store.
 * @return int The number of elements actually received.
 */
int recv_init_subset(double complex *data, int max);

/**
 * @brief Sends the results of the current node to the destination node.
 *
 * @param data The result data set to be sent.
 * @param size The number of elements in the result data set.
 * @param dest The ID number of the node to send the result data set to.
 */
void send_results(double complex *data, int size, int dest);

/**
 * @brief Recieves a result set from another node. There are no guarantees about
 * what order resulting sets will come in.
 *
 * @param data Pointer to buffer for holding the incoming data.
 * @param max The maximum amount the incoming data buffer can hold.
 * @return int The number of elements actually received.
 */
int recv_result_set(double complex *data, int max);

/**
 * @brief Stub function calling MPI_Barrier() and then MPI_Finalize(), does not
 * quit program.
 *
 */
void msg_finalize();

/**
 * @brief Aborts the current distributed calculation. Should only occur from an
 * unrecoverable error.
 *
 */
void msg_abort();

#endif  // MESSAGING_H_INCLUDED