/**
 * log.c - Logging utilities for LITE editor
 */

#include "lite.h"
#include "utils/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* Log file handle */
static FILE *log_fp = NULL;

/**
 * Initialize logging
 */
void log_init(const char *filename) {
    if (!filename) return;
    
    /* Open log file for writing */
    log_fp = fopen(filename, "w");
    if (!log_fp) {
        fprintf(stderr, "Failed to open log file: %s\n", filename);
        return;
    }
    
    /* Write initial log message */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[26];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log_fp, "[%s] [INFO] Log started\n", time_str);
    fflush(log_fp);
}

/**
 * Close logging
 */
void log_close(void) {
    if (log_fp) {
        /* Write final log message */
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char time_str[26];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        
        fprintf(log_fp, "[%s] [INFO] Log closed\n", time_str);
        
        fclose(log_fp);
        log_fp = NULL;
    }
}

/**
 * Write to log
 */
void log_write(LogLevel level, const char *file, int line, const char *fmt, ...) {
    if (!log_fp) return;
    
    /* Get current time */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[26];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    /* Level string */
    const char *level_str = "";
    switch (level) {
        case LOG_DEBUG:   level_str = "DEBUG"; break;
        case LOG_INFO:    level_str = "INFO"; break;
        case LOG_WARNING: level_str = "WARNING"; break;
        case LOG_ERROR:   level_str = "ERROR"; break;
    }
    
    /* Print log header */
    fprintf(log_fp, "[%s] [%s] %s:%d: ", time_str, level_str, file, line);
    
    /* Print log message */
    va_list args;
    va_start(args, fmt);
    vfprintf(log_fp, fmt, args);
    va_end(args);
    
    /* Add newline if needed */
    if (fmt[strlen(fmt) - 1] != '\n') {
        fprintf(log_fp, "\n");
    }
    
    /* Flush log */
    fflush(log_fp);
}