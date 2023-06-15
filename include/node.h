//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <stdbool.h>

/**
 * @brief This function contains all of the responsibilities of the head node,
 * which is assumed to have an ID of zero.
 *
 * @param filename The name of the input file.
 * @param header If true the first line of the input file will be ignored.
 * @param inverse If true perform the inverse FFT operation, otherwise the
 * forward FFT is used.
 */
void head_node(const char* filename, bool header, bool inverse);

/**
 * @brief This function contains the routines to be ran by all other nodes.
 * These nodes will be sent all of the information they need from the head node.
 *
 *  @param inverse If true perform the inverse FFT operation, otherwise the
 * forward FFT is used.
 */
void data_node(bool inverse);

#endif  // NODE_H_INCLUDED