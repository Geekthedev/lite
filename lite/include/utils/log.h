/**
 * log.h - Logging utilities for LITE editor
 */

#ifndef LITE_LOG_H
#define LITE_LOG_H

#include <stdio.h>

/* Log levels */
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

/* Log functions */
void log_init(const char *filename);
void log_close(void);
void log_write(LogLevel level, const char *file, int line, const char *fmt, ...);

/* Log macros */
#ifdef DEBUG
#define LOG_DEBUG(fmt, ...) log_write(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#define LOG_INFO(fmt, ...) log_write(LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) log_write(LOG_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_write(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif /* LITE_LOG_H */