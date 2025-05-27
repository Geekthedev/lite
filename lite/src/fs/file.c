/**
 * file.c - File operations for LITE editor
 */

#include "lite.h"
#include "fs/file.h"
#include "core/buffer.h"
#include "utils/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

/**
 * Load file into buffer
 */
int file_load(Buffer *buffer, const char *filename) {
    if (!buffer || !filename) return LITE_ERROR;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return LITE_ERROR_FILE_NOT_FOUND;
    }
    
    /* Clear buffer first */
    Line *line = buffer->first_line;
    while (line->next) {
        Line *next = line->next;
        free(line->data);
        free(line);
        line = next;
    }
    
    /* Free last line */
    free(line->data);
    
    /* Reset buffer state */
    buffer->first_line = NULL;
    buffer->current_line = NULL;
    buffer->line_count = 0;
    buffer->cursor_x = 0;
    buffer->cursor_y = 0;
    
    /* Read file line by line */
    char line_buf[LITE_MAX_LINE_LENGTH];
    Line *prev_line = NULL;
    
    while (fgets(line_buf, sizeof(line_buf), fp)) {
        /* Remove newline character */
        size_t len = strlen(line_buf);
        if (len > 0 && (line_buf[len - 1] == '\n' || line_buf[len - 1] == '\r')) {
            line_buf[len - 1] = '\0';
            len--;
        }
        if (len > 0 && line_buf[len - 1] == '\r') {
            line_buf[len - 1] = '\0';
            len--;
        }
        
        /* Create new line */
        Line *new_line = (Line*)malloc(sizeof(Line));
        if (!new_line) {
            fclose(fp);
            return LITE_ERROR;
        }
        
        /* Allocate and copy line data */
        new_line->data = strdup(line_buf);
        if (!new_line->data) {
            free(new_line);
            fclose(fp);
            return LITE_ERROR;
        }
        
        new_line->length = len;
        new_line->next = NULL;
        new_line->prev = prev_line;
        
        /* Link into list */
        if (prev_line) {
            prev_line->next = new_line;
        } else {
            buffer->first_line = new_line;
        }
        
        prev_line = new_line;
        buffer->line_count++;
    }
    
    /* If file was empty, create an empty line */
    if (buffer->line_count == 0) {
        Line *new_line = (Line*)malloc(sizeof(Line));
        if (!new_line) {
            fclose(fp);
            return LITE_ERROR;
        }
        
        new_line->data = strdup("");
        if (!new_line->data) {
            free(new_line);
            fclose(fp);
            return LITE_ERROR;
        }
        
        new_line->length = 0;
        new_line->next = NULL;
        new_line->prev = NULL;
        
        buffer->first_line = new_line;
        buffer->line_count = 1;
    }
    
    fclose(fp);
    
    /* Set current line to first line */
    buffer->current_line = buffer->first_line;
    
    /* Reset modified flag */
    buffer->modified = false;
    
    return LITE_OK;
}

/**
 * Save buffer to file
 */
int file_save(Buffer *buffer) {
    if (!buffer || !buffer->filename) return LITE_ERROR;
    
    FILE *fp = fopen(buffer->filename, "w");
    if (!fp) {
        return LITE_ERROR;
    }
    
    /* Write lines to file */
    Line *line = buffer->first_line;
    while (line) {
        fputs(line->data, fp);
        fputc('\n', fp);
        line = line->next;
    }
    
    fclose(fp);
    
    /* Reset modified flag */
    buffer->modified = false;
    
    return LITE_OK;
}

/**
 * Check if file exists
 */
int file_exists(const char *filename) {
    if (!filename) return 0;
    
    struct stat st;
    return (stat(filename, &st) == 0);
}

/**
 * Get absolute path for a filename
 */
char* file_get_absolute_path(const char *filename) {
    if (!filename) return NULL;
    
    char path_buf[PATH_MAX];
    
    if (filename[0] == '/') {
        /* Already absolute path */
        strncpy(path_buf, filename, PATH_MAX - 1);
    } else {
        /* Relative path, make absolute */
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return NULL;
        }
        
        snprintf(path_buf, sizeof(path_buf), "%s/%s", cwd, filename);
    }
    
    return strdup(path_buf);
}

/**
 * Get file extension
 */
char* file_get_extension(const char *filename) {
    if (!filename) return NULL;
    
    /* Find last dot in filename */
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return NULL;
    }
    
    /* Return extension including dot */
    return strdup(dot);
}