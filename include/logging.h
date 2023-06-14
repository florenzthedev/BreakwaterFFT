//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <stdio.h>

#define LOG_TRACE 6
#define LOG_DEBUG 5
#define LOG__INFO 4
#define LOG__WARN 3
#define LOG_ERROR 2
#define LOG_FATAL 1

#define LOG__ALL 10
#define LOG_NONE 0

extern char loglevel;
extern int lognode;
extern FILE *logger;

extern char *log_level_name[];

/**
 * @brief
 *
 * @param node_id
 * @param loglvl
 * @param logfile
 */
void init_log(int node_id, char loglvl);

/**
 * @brief Adds a log message. Will always start the message with "%s Node %i: "
 * where %s is the name of the log level and %i is the ID number registered with
 * log_init. If either of those have not yet been called no message will be
 * created. Will add a newline to the end of the message, will not add a period.
 *
 * Macro implementation, depends on C23 features.
 *
 */
#define log_msg(level, fmt, ...)                                        \
  if (level <= loglevel)                                                \
    fprintf(logger, "%s Node %i: " fmt "\n", log_level_name[level - 1], \
            lognode __VA_OPT__(, ) __VA_ARGS__);

#endif  // LOGGING_H_INCLUDED