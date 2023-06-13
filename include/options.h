//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

struct breakwater_options {
  int loglvl;
  int style;
  char *infilename;
  bool header;
};

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @param bopts
 */
void process_options(int argc, char *argv[], struct breakwater_options *bopts,
                     int node_id);

#endif  // OPTIONS_H_INCLUDED