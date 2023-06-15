//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

// TODO Add compatibility stuff for VSC++
#ifdef _DEBUG
#include <unistd.h>
#endif  //_DEBUG

#include "logging.h"
#include "messaging.h"
#include "node.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int node_id = msg_init(&argc, &argv);

  struct breakwater_options bopts;
  process_options(argc, argv, &bopts, node_id);

  init_log(node_id, bopts.loglvl);

#ifdef _DEBUG
  printf("Node %i waiting 10 seconds for debugger attachment.\n", node_id);
  sleep(10);
#endif  // debug

  log_msg(LOG__INFO, "Starting...");

  if (node_id == 0)
    head_node(bopts.infilename, bopts.header);
  else
    data_node(bopts.inverse);

  log_msg(LOG__INFO, "Finished!");
  msg_finalize();
  return 0;
}
