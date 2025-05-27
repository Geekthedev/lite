/**
 * main.c - Entry point for LITE editor
 * 
 * LITE (Lightweight Interface for Terminal Engineering)
 * A minimalist terminal-based code editor
 */

#include "lite.h"
#include "core/editor.h"
#include "utils/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>

/* Global editor state for signal handlers */
static EditorState *global_state = NULL;

/* Signal handler for cleanup */
static void handle_signal(int sig) {
    if (global_state) {
        editor_free(global_state);
    }
    
    /* Reset terminal */
    endwin();
    
    fprintf(stderr, "Received signal %d, exiting\n", sig);
    exit(1);
}

/* Print usage information */
static void print_usage(const char *program_name) {
    fprintf(stderr, "LITE Editor v%s\n", LITE_VERSION);
    fprintf(stderr, "Usage: %s [file...]\n", program_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help     Show this help message\n");
    fprintf(stderr, "  -v, --version  Show version information\n");
}

/* Print version information */
static void print_version(void) {
    fprintf(stderr, "LITE Editor v%s\n", LITE_VERSION);
    fprintf(stderr, "Lightweight Interface for Terminal Engineering\n");
    fprintf(stderr, "By %s\n", LITE_AUTHOR);
}

/* Main function */
int main(int argc, char *argv[]) {
    int i;
    
    /* Check for help or version flags */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        }
    }
    
    /* Set up locale */
    setlocale(LC_ALL, "");
    
    /* Initialize logging */
    log_init("lite.log");
    LOG_INFO("LITE Editor starting");
    
    /* Set up signal handlers */
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    /* Initialize editor */
    EditorState *state = editor_init();
    if (!state) {
        LOG_ERROR("Failed to initialize editor");
        return 1;
    }
    
    /* Store global reference for signal handlers */
    global_state = state;
    
    /* Open files from command line */
    for (i = 1; i < argc; i++) {
        if (editor_open_file(state, argv[i]) != LITE_OK) {
            LOG_WARNING("Failed to open file: %s", argv[i]);
        }
    }
    
    /* Create empty buffer if no files opened */
    if (state->buffer_count == 0) {
        Buffer *buffer = buffer_create();
        state->buffers[0] = buffer;
        state->buffer_count = 1;
        state->current_buffer = 0;
    }
    
    /* Main editor loop */
    state->running = true;
    while (state->running) {
        /* Render editor state */
        editor_render(state);
        
        /* Get and process user input */
        int key = ui_get_key(state);
        editor_process_key(state, key);
        
        /* Update editor state */
        editor_update(state);
    }
    
    /* Clean up */
    editor_free(state);
    global_state = NULL;
    
    LOG_INFO("LITE Editor exiting");
    log_close();
    
    return 0;
}