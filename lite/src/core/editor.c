/**
 * editor.c - Main editor state and operations
 */

#include "lite.h"
#include "core/editor.h"
#include "core/buffer.h"
#include "core/command.h"
#include "tui/ui.h"
#include "utils/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/**
 * Initialize the editor state
 */
EditorState* editor_init(void) {
    EditorState *state = (EditorState*)malloc(sizeof(EditorState));
    if (!state) return NULL;
    
    /* Initialize buffers */
    memset(state->buffers, 0, sizeof(state->buffers));
    state->buffer_count = 0;
    state->current_buffer = 0;
    
    /* Initialize mode */
    state->mode = MODE_NORMAL;
    
    /* Initialize command buffer */
    memset(state->command_buffer, 0, sizeof(state->command_buffer));
    state->command_pos = 0;
    
    /* Initialize status message */
    memset(state->status_message, 0, sizeof(state->status_message));
    state->status_message_time = 0;
    
    /* Initialize running state */
    state->running = false;
    
    /* Initialize configuration */
    state->config.tab_width = LITE_TAB_WIDTH;
    state->config.syntax_highlight = true;
    state->config.line_numbers = true;
    state->config.dark_mode = true;
    state->config.theme_name = strdup("default");
    state->config.config_path = strdup(LITE_CONFIG_FILE);
    
    /* Initialize UI */
    if (ui_init(state) != LITE_OK) {
        LOG_ERROR("Failed to initialize UI");
        free(state);
        return NULL;
    }
    
    /* Initialize commands */
    if (command_init() != LITE_OK) {
        LOG_ERROR("Failed to initialize commands");
        ui_free(state);
        free(state);
        return NULL;
    }
    
    /* Set initial status message */
    editor_set_status_message(state, "LITE Editor v%s | Press :q to quit", LITE_VERSION);
    
    return state;
}

/**
 * Free the editor state
 */
void editor_free(EditorState *state) {
    if (!state) return;
    
    /* Free UI state */
    ui_free(state);
    
    /* Free buffers */
    for (int i = 0; i < state->buffer_count; i++) {
        if (state->buffers[i]) {
            buffer_free(state->buffers[i]);
        }
    }
    
    /* Free configuration */
    if (state->config.theme_name) {
        free(state->config.theme_name);
    }
    
    if (state->config.config_path) {
        free(state->config.config_path);
    }
    
    /* Free editor state */
    free(state);
}

/**
 * Open a file in a new buffer
 */
int editor_open_file(EditorState *state, const char *filename) {
    if (!state || !filename) return LITE_ERROR;
    
    /* Check if buffer limit reached */
    if (state->buffer_count >= LITE_MAX_BUFFERS) {
        editor_set_status_message(state, "Buffer limit reached");
        return LITE_ERROR_BUFFER_FULL;
    }
    
    /* Create new buffer */
    Buffer *buffer = buffer_create();
    if (!buffer) {
        editor_set_status_message(state, "Failed to create buffer");
        return LITE_ERROR;
    }
    
    /* Load file into buffer */
    int result = buffer_load_file(buffer, filename);
    if (result != LITE_OK) {
        /* If file doesn't exist, keep empty buffer but set filename */
        if (result == LITE_ERROR_FILE_NOT_FOUND) {
            buffer->filename = strdup(filename);
            editor_set_status_message(state, "New file: %s", filename);
        } else {
            editor_set_status_message(state, "Failed to load file: %s", filename);
            buffer_free(buffer);
            return result;
        }
    } else {
        editor_set_status_message(state, "Opened %s", filename);
    }
    
    /* Add buffer to state */
    state->buffers[state->buffer_count] = buffer;
    state->current_buffer = state->buffer_count;
    state->buffer_count++;
    
    return LITE_OK;
}

/**
 * Save the current buffer
 */
int editor_save_current_buffer(EditorState *state) {
    if (!state || state->buffer_count == 0) return LITE_ERROR;
    
    Buffer *buffer = state->buffers[state->current_buffer];
    if (!buffer) return LITE_ERROR;
    
    /* If no filename, prompt for one */
    if (!buffer->filename) {
        editor_set_status_message(state, "No filename. Use :w <filename>");
        return LITE_ERROR;
    }
    
    /* Save buffer to file */
    int result = buffer_save_file(buffer);
    if (result != LITE_OK) {
        editor_set_status_message(state, "Failed to save file: %s", buffer->filename);
        return result;
    }
    
    editor_set_status_message(state, "Saved %s", buffer->filename);
    return LITE_OK;
}

/**
 * Switch to a different buffer
 */
int editor_switch_buffer(EditorState *state, int buffer_id) {
    if (!state) return LITE_ERROR;
    
    /* Find buffer with matching ID */
    for (int i = 0; i < state->buffer_count; i++) {
        if (state->buffers[i] && state->buffers[i]->id == buffer_id) {
            state->current_buffer = i;
            editor_set_status_message(state, "Switched to buffer %d", buffer_id);
            return LITE_OK;
        }
    }
    
    editor_set_status_message(state, "No buffer with ID %d", buffer_id);
    return LITE_ERROR;
}

/**
 * Close the current buffer
 */
int editor_close_current_buffer(EditorState *state) {
    if (!state || state->buffer_count == 0) return LITE_ERROR;
    
    Buffer *buffer = state->buffers[state->current_buffer];
    if (!buffer) return LITE_ERROR;
    
    /* Check if buffer is modified */
    if (buffer_is_modified(buffer)) {
        editor_set_status_message(state, "Buffer has unsaved changes. Use :q! to force quit");
        return LITE_ERROR;
    }
    
    /* Free buffer */
    buffer_free(buffer);
    
    /* Remove buffer from list */
    for (int i = state->current_buffer; i < state->buffer_count - 1; i++) {
        state->buffers[i] = state->buffers[i + 1];
    }
    
    state->buffer_count--;
    
    /* Adjust current buffer index */
    if (state->current_buffer >= state->buffer_count) {
        state->current_buffer = state->buffer_count - 1;
    }
    
    /* If no more buffers, quit */
    if (state->buffer_count == 0) {
        state->running = false;
        return LITE_OK;
    }
    
    editor_set_status_message(state, "Closed buffer");
    return LITE_OK;
}

/**
 * Set the editor mode
 */
void editor_set_mode(EditorState *state, EditorMode mode) {
    if (!state) return;
    
    state->mode = mode;
    
    /* Clear command buffer when exiting command mode */
    if (mode != MODE_COMMAND) {
        memset(state->command_buffer, 0, sizeof(state->command_buffer));
        state->command_pos = 0;
    }
}

/**
 * Process a keystroke
 */
void editor_process_key(EditorState *state, int key) {
    if (!state) return;
    
    Buffer *buffer = NULL;
    if (state->buffer_count > 0) {
        buffer = state->buffers[state->current_buffer];
    }
    
    /* Process key based on current mode */
    switch (state->mode) {
        case MODE_NORMAL:
            /* Normal mode keybindings */
            switch (key) {
                case 'h':
                    if (buffer) buffer_move_cursor(buffer, -1, 0);
                    break;
                    
                case 'j':
                    if (buffer) buffer_move_cursor(buffer, 0, 1);
                    break;
                    
                case 'k':
                    if (buffer) buffer_move_cursor(buffer, 0, -1);
                    break;
                    
                case 'l':
                    if (buffer) buffer_move_cursor(buffer, 1, 0);
                    break;
                    
                case 'i':
                    editor_set_mode(state, MODE_INSERT);
                    editor_set_status_message(state, "-- INSERT --");
                    break;
                    
                case ':':
                    editor_set_mode(state, MODE_COMMAND);
                    state->command_buffer[0] = '\0';
                    state->command_pos = 0;
                    break;
                    
                case 'q':
                    if (state->buffer_count > 0 && buffer_is_modified(buffer)) {
                        editor_set_status_message(state, "Buffer has unsaved changes. Use :q! to force quit");
                    } else {
                        state->running = false;
                    }
                    break;
            }
            break;
            
        case MODE_INSERT:
            /* Insert mode keybindings */
            switch (key) {
                case 27: /* ESC */
                    editor_set_mode(state, MODE_NORMAL);
                    editor_set_status_message(state, "-- NORMAL --");
                    break;
                    
                case KEY_BACKSPACE:
                case 127: /* DEL */
                    if (buffer) buffer_delete_char(buffer);
                    break;
                    
                case KEY_ENTER:
                case '\r':
                case '\n':
                    if (buffer) buffer_new_line(buffer);
                    break;
                    
                default:
                    if (key >= 32 && key < 127) {
                        if (buffer) buffer_insert_char(buffer, key);
                    }
                    break;
            }
            break;
            
        case MODE_COMMAND:
            /* Command mode keybindings */
            switch (key) {
                case 27: /* ESC */
                    editor_set_mode(state, MODE_NORMAL);
                    break;
                    
                case KEY_BACKSPACE:
                case 127: /* DEL */
                    if (state->command_pos > 0) {
                        state->command_pos--;
                        state->command_buffer[state->command_pos] = '\0';
                    }
                    break;
                    
                case KEY_ENTER:
                case '\r':
                case '\n':
                    /* Execute command */
                    if (strlen(state->command_buffer) > 0) {
                        command_execute(state, state->command_buffer);
                    }
                    editor_set_mode(state, MODE_NORMAL);
                    break;
                    
                default:
                    if (key >= 32 && key < 127 && state->command_pos < LITE_MAX_LINE_LENGTH - 1) {
                        state->command_buffer[state->command_pos] = (char)key;
                        state->command_pos++;
                        state->command_buffer[state->command_pos] = '\0';
                    }
                    break;
            }
            break;
            
        case MODE_VISUAL:
            /* Visual mode not implemented yet */
            editor_set_mode(state, MODE_NORMAL);
            break;
    }
}

/**
 * Update editor state
 */
void editor_update(EditorState *state) {
    if (!state) return;
    
    /* Check status message timeout */
    if (state->status_message_time > 0) {
        time_t current_time = time(NULL);
        if (current_time - state->status_message_time > 5) {
            state->status_message[0] = '\0';
            state->status_message_time = 0;
        }
    }
}

/**
 * Render the editor
 */
void editor_render(EditorState *state) {
    if (!state) return;
    
    ui_clear(state);
    
    /* Render buffer */
    ui_render_buffer(state);
    
    /* Render status line */
    ui_render_status_line(state);
    
    /* Render command line */
    ui_render_command_line(state);
    
    /* Render message */
    ui_render_message(state);
    
    /* Refresh display */
    ui_refresh(state);
}

/**
 * Set a status message
 */
void editor_set_status_message(EditorState *state, const char *fmt, ...) {
    if (!state) return;
    
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(state->status_message, sizeof(state->status_message), fmt, ap);
    va_end(ap);
    
    state->status_message_time = time(NULL);
}

/**
 * Execute a command
 */
int editor_execute_command(EditorState *state, const char *command) {
    if (!state || !command) return LITE_ERROR;
    
    return command_execute(state, command);
}

/**
 * Quit the editor
 */
void editor_quit(EditorState *state) {
    if (!state) return;
    
    state->running = false;
}

/**
 * Load editor configuration
 */
int editor_load_config(EditorState *state, const char *config_path) {
    if (!state || !config_path) return LITE_ERROR;
    
    /* TODO: Implement configuration loading */
    editor_set_status_message(state, "Config loading not implemented yet");
    
    return LITE_OK;
}

/**
 * Save editor configuration
 */
int editor_save_config(EditorState *state) {
    if (!state) return LITE_ERROR;
    
    /* TODO: Implement configuration saving */
    editor_set_status_message(state, "Config saving not implemented yet");
    
    return LITE_OK;
}