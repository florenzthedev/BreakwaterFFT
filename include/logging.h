//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <stdio.h>

#define LOG_INIT 0b10000000
#define LOG_MSSG 0b01000000
#define LOG_CALC 0b00100000

#define LOG_ALL 0b11111111

extern int lognode;
extern char loglevel;
extern FILE *logger;

#define SET_LOGNODE(nodeid) lognode = nodeid;
#define SET_LOGLEVEL(logmask) loglevel = logmask;
#define SET_LOGFILEP(logfile) logger = logfile;

#define LOG(level, fmt, ...) \
  if (level | loglevel) fprintf(logger, fmt, __VA_ARGS__);

#endif  // LOGGING_H_INCLUDED