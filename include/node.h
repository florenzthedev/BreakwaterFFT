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
 */
void head_node(const char* filename, bool header);

/**
 * @brief This function contains the routines to be ran by all other nodes.
 * These nodes will be send all of the information they need from the head node.
 *
 */
void data_node(bool inverse);

#endif  // NODE_H_INCLUDED