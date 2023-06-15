//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

struct breakwater_options {
  char *infilename;
  int loglvl;
  int style;
  bool header;
  bool inverse;
  bool use_lut;
};

/**
 * @brief This processes the command line options to our program. This function
 * is designed to be called after msg_init and assumes that it is running on all
 * nodes. Only the head node will print out non-standard messages and if any
 * option needs to close the program it will close all nodes.
 *
 * @param argc Argument count from main.
 * @param argv Argument vector from main.
 * @param bopts breakwater_options struct to store the results of the command
 * line options in.
 */
void process_options(int argc, char *argv[], struct breakwater_options *bopts,
                     int node_id);

#endif  // OPTIONS_H_INCLUDED