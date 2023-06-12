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
#define LOG_DFLT 5
#define LOG_NONE 0

/**
 * @brief
 *
 * @param node_id
 * @param loglvl
 */
void init_log(int node_id, char loglvl);

/**
 * @brief
 *
 * @param node_id
 * @param loglvl
 * @param logfile
 */
void init_log_file(int node_id, char loglvl, FILE *logfile);

/**
 * @brief Adds a log message. Will always start the message with "%s Node %i: "
 * where %s is the name of the log level and %i is the ID number registered with
 * log_init or log_init_file. If either of those have not yet been called no
 * message will be created. Will add a newline to the end of the message, will
 * not add a period.
 *
 * @param level Level of log message.
 * @param fmt Formatted string with message.
 * @param ... Additional variables for the message.
 */
void log_msg(int level, const char *fmt, ...);

#endif  // LOGGING_H_INCLUDED