//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include "options.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "messaging.h"

void print_help() {}

void default_options(struct breakwater_options *bopts) {
  bopts->loglvl = 4;
  bopts->style = 1;
  bopts->infilename = NULL;
  bopts->header = false;
}

void process_options(int argc, char *argv[], struct breakwater_options *bopts,
                     int node_id) {
  int temp = 0, carg;
  default_options(bopts);
  while ((carg = getopt(argc, argv, "hl:d")) != -1) {
    switch (carg) {
      case 'h':
        if (node_id == 0) print_help();
        msg_finalize();
        exit(EXIT_SUCCESS);

      case 'l':
        temp = strtol(optarg, NULL, 10);
        if (temp == 0 && optarg[0] != '0') {
          if (node_id == 0)
            fprintf(stderr, "Error: invalid loglevel: %s\n", optarg);
          msg_finalize();
          exit(EXIT_FAILURE);
        }
        bopts->loglvl = temp;
        break;

      case 'd':
        bopts->header = true;
        break;

      case '?':
        // Error message already printed out
        msg_finalize();
        exit(EXIT_FAILURE);

      default:
        abort();
    }
  }

  bopts->infilename = argv[optind];
}