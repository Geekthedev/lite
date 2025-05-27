/**
 * buffer.c - Text buffer handling for LITE editor
 */

#include "lite.h"
#include "core/buffer.h"
#include "fs/file.h"
#include "utils/log.h"
#include <stdlib.h>
#include <string.h>

/* Static buffer ID counter */
static int next_buffer_id = 1;

/**
 * Create a new empty line
 */
static Line* create_line(void) {
    Line *line = (Line*)malloc(sizeof(Line));
    if (!line) return NULL;
    
    line->data = (char*)malloc(1);
    if (!line->data) {
        free(line);
        return NULL;
    }
    
    line->data[0] = '\0';
    line->length = 0;
    line->prev = NULL;
    line->next = NULL;
    
    return line;
}

/**
 * Free a line and its data
 */
static void free_line(Line *line) {
    if (!line) return;
    
    if (line->data) {
        free(line->data);
    }
    
    free(line);
}

/**
 * Create a new empty buffer
 */
Buffer* buffer_create(void) {
    Buffer *buffer = (Buffer*)malloc(sizeof(Buffer));
    if (!buffer) return NULL;
    
    buffer->filename = NULL;
    buffer->cursor_x = 0;
    buffer->cursor_y = 0;
    buffer->scroll_x = 0;
    buffer->scroll_y = 0;
    buffer->modified = false;
    buffer->id = next_buffer_id++;
    
    /* Create initial empty line */
    buffer->first_line = create_line();
    if (!buffer->first_line) {
        free(buffer);
        return NULL;
    }
    
    buffer->current_line = buffer->first_line;
    buffer->line_count = 1;
    
    return buffer;
}

/**
 * Free a buffer and all its lines
 */
void buffer_free(Buffer *buffer) {
    if (!buffer) return;
    
    /* Free all lines */
    Line *line = buffer->first_line;
    while (line) {
        Line *next = line->next;
        free_line(line);
        line = next;
    }
    
    /* Free filename */
    if (buffer->filename) {
        free(buffer->filename);
    }
    
    /* Free buffer */
    free(buffer);
}

/**
 * Load a file into a buffer
 */
int buffer_load_file(Buffer *buffer, const char *filename) {
    if (!buffer || !filename) return LITE_ERROR;
    
    int result = file_load(buffer, filename);
    if (result == LITE_OK) {
        /* Store filename */
        if (buffer->filename) {
            free(buffer->filename);
        }
        
        buffer->filename = strdup(filename);
        buffer->modified = false;
    }
    
    return result;
}

/**
 * Save a buffer to its file
 */
int buffer_save_file(Buffer *buffer) {
    if (!buffer) return LITE_ERROR;
    if (!buffer->filename) return LITE_ERROR;
    
    int result = file_save(buffer);
    if (result == LITE_OK) {
        buffer->modified = false;
    }
    
    return result;
}

/**
 * Insert a character at the current cursor position
 */
int buffer_insert_char(Buffer *buffer, int ch) {
    if (!buffer || !buffer->current_line) return LITE_ERROR;
    
    Line *line = buffer->current_line;
    int pos = buffer->cursor_x;
    
    /* Allocate new memory for the line */
    char *new_data = (char*)realloc(line->data, line->length + 2);
    if (!new_data) return LITE_ERROR;
    
    line->data = new_data;
    
    /* Shift characters after the cursor */
    memmove(line->data + pos + 1, line->data + pos, line->length - pos + 1);
    
    /* Insert the character */
    line->data[pos] = (char)ch;
    line->length++;
    
    /* Move cursor right */
    buffer->cursor_x++;
    
    /* Mark buffer as modified */
    buffer->modified = true;
    
    return LITE_OK;
}

/**
 * Delete the character before the cursor
 */
int buffer_delete_char(Buffer *buffer) {
    if (!buffer || !buffer->current_line) return LITE_ERROR;
    
    Line *line = buffer->current_line;
    int pos = buffer->cursor_x;
    
    /* Can't delete at beginning of line */
    if (pos == 0) {
        /* If not first line, merge with previous line */
        if (line->prev) {
            Line *prev_line = line->prev;
            int prev_length = prev_line->length;
            
            /* Resize previous line to fit both */
            char *new_data = (char*)realloc(prev_line->data, prev_length + line->length + 1);
            if (!new_data) return LITE_ERROR;
            
            prev_line->data = new_data;
            
            /* Copy current line to end of previous line */
            memcpy(prev_line->data + prev_length, line->data, line->length + 1);
            prev_line->length += line->length;
            
            /* Update line links */
            prev_line->next = line->next;
            if (line->next) {
                line->next->prev = prev_line;
            }
            
            /* Update buffer state */
            buffer->current_line = prev_line;
            buffer->cursor_x = prev_length;
            buffer->cursor_y--;
            buffer->line_count--;
            
            /* Free the current line */
            free_line(line);
        } else {
            return LITE_OK; /* Can't delete at beginning of first line */
        }
    } else {
        /* Delete character within line */
        memmove(line->data + pos - 1, line->data + pos, line->length - pos + 1);
        line->length--;
        
        /* Move cursor left */
        buffer->cursor_x--;
    }
    
    /* Mark buffer as modified */
    buffer->modified = true;
    
    return LITE_OK;
}

/**
 * Insert a new line at the current cursor position
 */
int buffer_new_line(Buffer *buffer) {
    if (!buffer || !buffer->current_line) return LITE_ERROR;
    
    Line *current = buffer->current_line;
    int pos = buffer->cursor_x;
    
    /* Create new line */
    Line *new_line = create_line();
    if (!new_line) return LITE_ERROR;
    
    /* If splitting line, copy text after cursor to new line */
    if (pos < current->length) {
        new_line->data = (char*)realloc(new_line->data, current->length - pos + 1);
        if (!new_line->data) {
            free(new_line);
            return LITE_ERROR;
        }
        
        strcpy(new_line->data, current->data + pos);
        new_line->length = current->length - pos;
        
        /* Truncate current line */
        current->data[pos] = '\0';
        current->length = pos;
    }
    
    /* Link the new line into the list */
    new_line->next = current->next;
    new_line->prev = current;
    
    if (current->next) {
        current->next->prev = new_line;
    }
    
    current->next = new_line;
    
    /* Update buffer state */
    buffer->current_line = new_line;
    buffer->cursor_x = 0;
    buffer->cursor_y++;
    buffer->line_count++;
    
    /* Mark buffer as modified */
    buffer->modified = true;
    
    return LITE_OK;
}

/**
 * Move the cursor by a relative amount
 */
void buffer_move_cursor(Buffer *buffer, int dx, int dy) {
    if (!buffer) return;
    
    /* Move vertically */
    while (dy < 0 && buffer->current_line->prev) {
        buffer->current_line = buffer->current_line->prev;
        buffer->cursor_y--;
        dy++;
    }
    
    while (dy > 0 && buffer->current_line->next) {
        buffer->current_line = buffer->current_line->next;
        buffer->cursor_y++;
        dy--;
    }
    
    /* Move horizontally */
    buffer->cursor_x += dx;
    
    /* Clamp cursor position */
    if (buffer->cursor_x < 0) {
        buffer->cursor_x = 0;
    }
    
    if (buffer->cursor_x > buffer->current_line->length) {
        buffer->cursor_x = buffer->current_line->length;
    }
}

/**
 * Set the cursor to an absolute position
 */
void buffer_set_cursor(Buffer *buffer, int x, int y) {
    if (!buffer) return;
    
    /* Start from first line */
    buffer->current_line = buffer->first_line;
    buffer->cursor_y = 0;
    
    /* Move to target line */
    while (buffer->cursor_y < y && buffer->current_line->next) {
        buffer->current_line = buffer->current_line->next;
        buffer->cursor_y++;
    }
    
    /* Set x position */
    buffer->cursor_x = x;
    
    /* Clamp cursor position */
    if (buffer->cursor_x < 0) {
        buffer->cursor_x = 0;
    }
    
    if (buffer->cursor_x > buffer->current_line->length) {
        buffer->cursor_x = buffer->current_line->length;
    }
}

/**
 * Get the text of the current line
 */
char* buffer_get_current_line(Buffer *buffer) {
    if (!buffer || !buffer->current_line) return NULL;
    return buffer->current_line->data;
}

/**
 * Get the number of lines in the buffer
 */
int buffer_get_line_count(Buffer *buffer) {
    if (!buffer) return 0;
    return buffer->line_count;
}

/**
 * Check if the buffer has been modified
 */
bool buffer_is_modified(Buffer *buffer) {
    if (!buffer) return false;
    return buffer->modified;
}