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

#ifndef __GNUC__
void log_msg(int level, const char *fmt, ...) {
  // TODO Explore macro options to make this faster
  if (level > loglevel) return;
  va_list args;
  va_start(args, fmt);
  fprintf(logger, "%s Node %i: ", log_level_name[level - 1], lognode);
  vfprintf(logger, fmt, args);
  putc('\n', logger);
  va_end(args);
}
#endif  // __GNUC__