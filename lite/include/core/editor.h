/**
 * editor.h - Main editor state and operations
 */

#ifndef LITE_EDITOR_H
#define LITE_EDITOR_H

#include "buffer.h"
#include "../tui/ui.h"

/* Editor configuration */
typedef struct EditorConfig {
    int tab_width;
    bool syntax_highlight;
    bool line_numbers;
    bool dark_mode;
    char *theme_name;
    char *config_path;
} EditorConfig;

/* Editor state */
typedef struct EditorState {
    Buffer *buffers[LITE_MAX_BUFFERS];
    int buffer_count;
    int current_buffer;
    EditorMode mode;
    EditorConfig config;
    UIState ui;
    char command_buffer[LITE_MAX_LINE_LENGTH];
    int command_pos;
    bool running;
    char status_message[LITE_MAX_LINE_LENGTH];
    int status_message_time;
} EditorState;

/* Editor functions */
EditorState* editor_init(void);
void editor_free(EditorState *state);
int editor_open_file(EditorState *state, const char *filename);
int editor_save_current_buffer(EditorState *state);
int editor_switch_buffer(EditorState *state, int buffer_id);
int editor_close_current_buffer(EditorState *state);
void editor_set_mode(EditorState *state, EditorMode mode);
void editor_process_key(EditorState *state, int key);
void editor_update(EditorState *state);
void editor_render(EditorState *state);
void editor_set_status_message(EditorState *state, const char *fmt, ...);
int editor_execute_command(EditorState *state, const char *command);
void editor_quit(EditorState *state);
int editor_load_config(EditorState *state, const char *config_path);
int editor_save_config(EditorState *state);

#endif /* LITE_EDITOR_H */