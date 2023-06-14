//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

char loglevel = LOG_NONE;
int lognode = -1;
FILE *logger;

char *log_level_name[] = {"FATAL  ", "ERROR  ", "WARNING",
                          "INFO   ", "DEBUG  ", "TRACE  "};

void init_log(int node_id, char loglvl) {
  lognode = node_id;
  loglevel = loglvl;
  logger = stderr;
}