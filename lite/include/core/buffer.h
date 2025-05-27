/**
 * buffer.h - Text buffer handling for LITE editor
 */

#ifndef LITE_BUFFER_H
#define LITE_BUFFER_H

#include <stdbool.h>

/* Forward declarations */
struct EditorState;

/* Line structure */
typedef struct Line {
    char *data;
    int length;
    struct Line *prev;
    struct Line *next;
} Line;

/* Buffer structure */
typedef struct Buffer {
    char *filename;
    Line *first_line;
    Line *current_line;
    int line_count;
    int cursor_x;
    int cursor_y;
    int scroll_x;
    int scroll_y;
    bool modified;
    int id;
} Buffer;

/* Buffer functions */
Buffer* buffer_create(void);
void buffer_free(Buffer *buffer);
int buffer_load_file(Buffer *buffer, const char *filename);
int buffer_save_file(Buffer *buffer);
int buffer_insert_char(Buffer *buffer, int ch);
int buffer_delete_char(Buffer *buffer);
int buffer_new_line(Buffer *buffer);
void buffer_move_cursor(Buffer *buffer, int dx, int dy);
void buffer_set_cursor(Buffer *buffer, int x, int y);
char* buffer_get_current_line(Buffer *buffer);
int buffer_get_line_count(Buffer *buffer);
bool buffer_is_modified(Buffer *buffer);

#endif /* LITE_BUFFER_H */