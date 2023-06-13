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
  int node_id;

  node_id = msg_init(&argc, &argv);

  init_log(node_id, LOG__ALL);

#ifdef _DEBUG
  printf("Node %i waiting 10 seconds for debugger attachment.\n", node_id);
  sleep(10);
#endif  // debug

  log_msg(LOG__INFO, "Starting...");

  if (node_id == 0)
    head_node(argv[1]);
  else
    data_node(node_id);

  log_msg(LOG__INFO, "Finished!");
  msg_finalize();
  return 0;
}
