/**
 * file.h - File operations for LITE editor
 */

#ifndef LITE_FILE_H
#define LITE_FILE_H

#include "../core/buffer.h"

/* File operations */
int file_load(Buffer *buffer, const char *filename);
int file_save(Buffer *buffer);
int file_exists(const char *filename);
char* file_get_absolute_path(const char *filename);
char* file_get_extension(const char *filename);

#endif /* LITE_FILE_H */