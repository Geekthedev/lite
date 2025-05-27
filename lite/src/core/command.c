/**
 * command.c - Command processing for LITE editor
 */

#include "lite.h"
#include "core/command.h"
#include "core/editor.h"
#include "utils/log.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Maximum number of commands */
#define MAX_COMMANDS 32

/* Maximum number of arguments */
#define MAX_ARGS 16

/* Command registry */
static Command commands[MAX_COMMANDS];
static int command_count = 0;

/**
 * Parse command line into arguments
 */
static int parse_args(char *cmd, char **argv) {
    int argc = 0;
    char *p = cmd;
    
    /* Skip leading whitespace */
    while (isspace(*p)) p++;
    
    /* Parse arguments */
    while (*p && argc < MAX_ARGS) {
        argv[argc++] = p;
        
        /* Find end of argument */
        while (*p && !isspace(*p)) p++;
        
        /* Null-terminate argument */
        if (*p) {
            *p++ = '\0';
            
            /* Skip whitespace between arguments */
            while (isspace(*p)) p++;
        }
    }
    
    return argc;
}

/**
 * Initialize command system
 */
int command_init(void) {
    /* Register built-in commands */
    command_register("open", "Open a file for editing", command_open);
    command_register("write", "Save the current buffer", command_write);
    command_register("quit", "Exit the editor", command_quit);
    command_register("tab", "Tab management", command_tab);
    command_register("theme", "Theme management", command_theme);
    command_register("help", "Show help", command_help);
    
    return LITE_OK;
}

/**
 * Register a new command
 */
int command_register(const char *name, const char *help, CommandFunc func) {
    if (!name || !func) return LITE_ERROR;
    
    /* Check if command already exists */
    for (int i = 0; i < command_count; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            commands[i].help = help;
            commands[i].func = func;
            return LITE_OK;
        }
    }
    
    /* Check if command registry is full */
    if (command_count >= MAX_COMMANDS) {
        return LITE_ERROR;
    }
    
    /* Add new command */
    commands[command_count].name = name;
    commands[command_count].help = help;
    commands[command_count].func = func;
    command_count++;
    
    return LITE_OK;
}

/**
 * Execute a command
 */
int command_execute(EditorState *state, const char *command_line) {
    if (!state || !command_line) return LITE_ERROR;
    
    /* Copy command line to avoid modifying the original */
    char cmd_copy[LITE_MAX_LINE_LENGTH];
    strncpy(cmd_copy, command_line, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';
    
    /* Parse arguments */
    char *argv[MAX_ARGS];
    int argc = parse_args(cmd_copy, argv);
    
    if (argc == 0) {
        editor_set_status_message(state, "Empty command");
        return LITE_ERROR;
    }
    
    /* Find command */
    for (int i = 0; i < command_count; i++) {
        if (strcmp(commands[i].name, argv[0]) == 0) {
            /* Execute command */
            return commands[i].func(state, argc, argv);
        }
    }
    
    editor_set_status_message(state, "Unknown command: %s", argv[0]);
    return LITE_ERROR;
}

/**
 * Show help for a command
 */
void command_show_help(EditorState *state, const char *command_name) {
    if (!state) return;
    
    /* If no command name, show all commands */
    if (!command_name) {
        editor_set_status_message(state, "Available commands:");
        
        /* TODO: Show in a buffer instead of status line */
        return;
    }
    
    /* Find command */
    for (int i = 0; i < command_count; i++) {
        if (strcmp(commands[i].name, command_name) == 0) {
            editor_set_status_message(state, "%s: %s", commands[i].name, commands[i].help);
            return;
        }
    }
    
    editor_set_status_message(state, "Unknown command: %s", command_name);
}

/**
 * Built-in command: open
 */
int command_open(EditorState *state, int argc, char **argv) {
    if (!state) return LITE_ERROR;
    
    if (argc < 2) {
        editor_set_status_message(state, "Usage: open <filename>");
        return LITE_ERROR;
    }
    
    return editor_open_file(state, argv[1]);
}

/**
 * Built-in command: write
 */
int command_write(EditorState *state, int argc, char **argv) {
    if (!state) return LITE_ERROR;
    
    /* If filename provided, set it for current buffer */
    if (argc >= 2 && state->buffer_count > 0) {
        Buffer *buffer = state->buffers[state->current_buffer];
        if (buffer) {
            if (buffer->filename) {
                free(buffer->filename);
            }
            buffer->filename = strdup(argv[1]);
        }
    }
    
    return editor_save_current_buffer(state);
}

/**
 * Built-in command: quit
 */
int command_quit(EditorState *state, int argc, char **argv) {
    if (!state) return LITE_ERROR;
    
    /* Check for force flag */
    bool force = false;
    if (argc >= 2 && strcmp(argv[1], "!") == 0) {
        force = true;
    }
    
    /* Check for unsaved changes */
    if (!force && state->buffer_count > 0) {
        Buffer *buffer = state->buffers[state->current_buffer];
        if (buffer && buffer_is_modified(buffer)) {
            editor_set_status_message(state, "Buffer has unsaved changes. Use :q! to force quit");
            return LITE_ERROR;
        }
    }
    
    /* Close current buffer or quit */
    if (state->buffer_count > 1) {
        return editor_close_current_buffer(state);
    } else {
        editor_quit(state);
        return LITE_OK;
    }
}

/**
 * Built-in command: tab
 */
int command_tab(EditorState *state, int argc, char **argv) {
    if (!state) return LITE_ERROR;
    
    if (argc < 2) {
        editor_set_status_message(state, "Usage: tab new|list|<id>");
        return LITE_ERROR;
    }
    
    if (strcmp(argv[1], "new") == 0) {
        /* Create new empty buffer */
        Buffer *buffer = buffer_create();
        if (!buffer) {
            editor_set_status_message(state, "Failed to create buffer");
            return LITE_ERROR;
        }
        
        /* Add buffer to state */
        if (state->buffer_count >= LITE_MAX_BUFFERS) {
            editor_set_status_message(state, "Buffer limit reached");
            buffer_free(buffer);
            return LITE_ERROR;
        }
        
        state->buffers[state->buffer_count] = buffer;
        state->current_buffer = state->buffer_count;
        state->buffer_count++;
        
        editor_set_status_message(state, "New buffer created");
        return LITE_OK;
    } else if (strcmp(argv[1], "list") == 0) {
        /* List buffers */
        editor_set_status_message(state, "Buffer list not implemented yet");
        return LITE_OK;
    } else {
        /* Switch to buffer by ID */
        int id = atoi(argv[1]);
        return editor_switch_buffer(state, id);
    }
}

/**
 * Built-in command: theme
 */
int command_theme(EditorState *state, int argc, char **argv) {
    if (!state) return LITE_ERROR;
    
    if (argc < 2) {
        editor_set_status_message(state, "Usage: theme load <name>");
        return LITE_ERROR;
    }
    
    if (strcmp(argv[1], "load") == 0) {
        if (argc < 3) {
            editor_set_status_message(state, "Usage: theme load <name>");
            return LITE_ERROR;
        }
        
        /* TODO: Implement theme loading */
        editor_set_status_message(state, "Theme loading not implemented yet");
        return LITE_OK;
    }
    
    editor_set_status_message(state, "Unknown theme command: %s", argv[1]);
    return LITE_ERROR;
}

/**
 * Built-in command: help
 */
int command_help(EditorState *state, int argc, char **argv) {
    if (!state) return LITE_ERROR;
    
    if (argc < 2) {
        /* Show general help */
        command_show_help(state, NULL);
    } else {
        /* Show help for specific command */
        command_show_help(state, argv[1]);
    }
    
    return LITE_OK;
}