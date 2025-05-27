/**
 * ui.c - Terminal UI handling for LITE editor
 */

#include "lite.h"
#include "tui/ui.h"
#include "core/editor.h"
#include "core/buffer.h"
#include "syntax/highlight.h"
#include "utils/log.h"
#include <stdlib.h>
#include <string.h>

/**
 * Initialize the UI
 */
int ui_init(EditorState *state) {
    if (!state) return LITE_ERROR;
    
    /* Initialize ncurses */
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    timeout(100); /* Non-blocking input with 100ms timeout */
    
    /* Enable colors if available */
    if (has_colors()) {
        start_color();
        use_default_colors();
        
        /* Initialize color pairs */
        init_pair(1, COLOR_WHITE, COLOR_BLACK);   /* Default */
        init_pair(2, COLOR_GREEN, COLOR_BLACK);   /* Keywords */
        init_pair(3, COLOR_CYAN, COLOR_BLACK);    /* Types */
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);  /* Strings */
        init_pair(5, COLOR_BLUE, COLOR_BLACK);    /* Comments */
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK); /* Numbers */
        init_pair(7, COLOR_WHITE, COLOR_BLACK);   /* Identifiers */
        init_pair(8, COLOR_RED, COLOR_BLACK);     /* Preprocessor */
        init_pair(9, COLOR_GREEN, COLOR_BLACK);   /* Operators */
        init_pair(10, COLOR_BLACK, COLOR_WHITE);  /* Status line */
    }
    
    /* Get terminal size */
    getmaxyx(stdscr, state->ui.term_height, state->ui.term_width);
    
    /* Create windows */
    state->ui.main_win = newwin(state->ui.term_height - 2, state->ui.term_width, 0, 0);
    state->ui.status_win = newwin(1, state->ui.term_width, state->ui.term_height - 2, 0);
    state->ui.command_win = newwin(1, state->ui.term_width, state->ui.term_height - 1, 0);
    
    /* Enable keypad for all windows */
    keypad(state->ui.main_win, TRUE);
    keypad(state->ui.status_win, TRUE);
    keypad(state->ui.command_win, TRUE);
    
    /* Set editor height */
    state->ui.editor_height = state->ui.term_height - 2;
    
    return LITE_OK;
}

/**
 * Free UI resources
 */
void ui_free(EditorState *state) {
    if (!state) return;
    
    /* Delete windows */
    if (state->ui.main_win) delwin(state->ui.main_win);
    if (state->ui.status_win) delwin(state->ui.status_win);
    if (state->ui.command_win) delwin(state->ui.command_win);
    
    /* End ncurses */
    endwin();
}

/**
 * Handle terminal resize
 */
void ui_resize(EditorState *state) {
    if (!state) return;
    
    /* Get new terminal size */
    getmaxyx(stdscr, state->ui.term_height, state->ui.term_width);
    
    /* Resize windows */
    wresize(state->ui.main_win, state->ui.term_height - 2, state->ui.term_width);
    wresize(state->ui.status_win, 1, state->ui.term_width);
    wresize(state->ui.command_win, 1, state->ui.term_width);
    
    /* Move windows */
    mvwin(state->ui.status_win, state->ui.term_height - 2, 0);
    mvwin(state->ui.command_win, state->ui.term_height - 1, 0);
    
    /* Update editor height */
    state->ui.editor_height = state->ui.term_height - 2;
    
    /* Redraw */
    redrawwin(state->ui.main_win);
    redrawwin(state->ui.status_win);
    redrawwin(state->ui.command_win);
}

/**
 * Render buffer content
 */
void ui_render_buffer(EditorState *state) {
    if (!state) return;
    
    WINDOW *win = state->ui.main_win;
    
    /* Clear window */
    werase(win);
    
    /* If no buffer, show welcome message */
    if (state->buffer_count == 0) {
        char welcome[80];
        int welcome_len = snprintf(welcome, sizeof(welcome),
                                  "LITE Editor v%s", LITE_VERSION);
        
        int welcome_x = (state->ui.term_width - welcome_len) / 2;
        int welcome_y = state->ui.term_height / 3;
        
        mvwprintw(win, welcome_y, welcome_x, "%s", welcome);
        mvwprintw(win, welcome_y + 2, welcome_x - 10, "Type :help for help, :q to quit");
        
        return;
    }
    
    /* Get current buffer */
    Buffer *buffer = state->buffers[state->current_buffer];
    if (!buffer) return;
    
    /* Determine display range */
    int start_y = buffer->scroll_y;
    int end_y = start_y + state->ui.editor_height;
    
    /* Display buffer content */
    Line *line = buffer->first_line;
    int line_num = 0;
    
    /* Skip lines before scroll position */
    while (line && line_num < start_y) {
        line = line->next;
        line_num++;
    }
    
    /* Display visible lines */
    int y = 0;
    while (line && line_num < end_y) {
        /* Display line number if enabled */
        if (state->config.line_numbers) {
            wattron(win, A_DIM);
            mvwprintw(win, y, 0, "%3d ", line_num + 1);
            wattroff(win, A_DIM);
        }
        
        /* Display line content */
        int x_offset = state->config.line_numbers ? 4 : 0;
        if (state->config.syntax_highlight) {
            /* TODO: Implement proper syntax highlighting */
            mvwprintw(win, y, x_offset, "%s", line->data);
        } else {
            mvwprintw(win, y, x_offset, "%s", line->data);
        }
        
        line = line->next;
        line_num++;
        y++;
    }
    
    /* Position cursor */
    int cursor_x = buffer->cursor_x - buffer->scroll_x;
    int cursor_y = buffer->cursor_y - buffer->scroll_y;
    
    if (state->config.line_numbers) {
        cursor_x += 4;
    }
    
    wmove(win, cursor_y, cursor_x);
}

/**
 * Render status line
 */
void ui_render_status_line(EditorState *state) {
    if (!state) return;
    
    WINDOW *win = state->ui.status_win;
    
    /* Clear window */
    werase(win);
    
    /* Set status line color */
    wattron(win, COLOR_PAIR(10));
    
    /* Fill entire line */
    for (int i = 0; i < state->ui.term_width; i++) {
        mvwprintw(win, 0, i, " ");
    }
    
    /* If no buffer, show basic status */
    if (state->buffer_count == 0) {
        mvwprintw(win, 0, 0, " LITE Editor");
        mvwprintw(win, 0, state->ui.term_width - 12, "No File");
        wattroff(win, COLOR_PAIR(10));
        return;
    }
    
    /* Get current buffer */
    Buffer *buffer = state->buffers[state->current_buffer];
    if (!buffer) {
        wattroff(win, COLOR_PAIR(10));
        return;
    }
    
    /* Left side: filename and modified indicator */
    char left_status[256];
    char *filename = buffer->filename ? buffer->filename : "[No Name]";
    snprintf(left_status, sizeof(left_status), " %s%s",
             filename, buffer->modified ? " [+]" : "");
    
    /* Right side: position information */
    char right_status[64];
    snprintf(right_status, sizeof(right_status), "%d:%d | %d lines ",
             buffer->cursor_y + 1, buffer->cursor_x + 1, buffer->line_count);
    
    /* Display status */
    mvwprintw(win, 0, 0, "%s", left_status);
    mvwprintw(win, 0, state->ui.term_width - strlen(right_status), "%s", right_status);
    
    /* Display mode indicator in middle */
    char mode_str[16] = "";
    switch (state->mode) {
        case MODE_NORMAL: strcpy(mode_str, "NORMAL"); break;
        case MODE_INSERT: strcpy(mode_str, "INSERT"); break;
        case MODE_COMMAND: strcpy(mode_str, "COMMAND"); break;
        case MODE_VISUAL: strcpy(mode_str, "VISUAL"); break;
    }
    
    int mode_x = (state->ui.term_width - strlen(mode_str)) / 2;
    mvwprintw(win, 0, mode_x, "%s", mode_str);
    
    /* Reset color */
    wattroff(win, COLOR_PAIR(10));
}

/**
 * Render command line
 */
void ui_render_command_line(EditorState *state) {
    if (!state) return;
    
    WINDOW *win = state->ui.command_win;
    
    /* Clear window */
    werase(win);
    
    /* If in command mode, show command input */
    if (state->mode == MODE_COMMAND) {
        mvwprintw(win, 0, 0, ":%s", state->command_buffer);
        wmove(win, 0, state->command_pos + 1); /* +1 for the colon */
    } else if (strlen(state->status_message) > 0) {
        /* Show status message */
        mvwprintw(win, 0, 0, "%s", state->status_message);
    }
}

/**
 * Render message
 */
void ui_render_message(EditorState *state) {
    if (!state || state->mode == MODE_COMMAND) return;
    
    /* If not in command mode and we have a status message, show it */
    if (strlen(state->status_message) > 0) {
        mvwprintw(state->ui.command_win, 0, 0, "%s", state->status_message);
    }
}

/**
 * Refresh display
 */
void ui_refresh(EditorState *state) {
    if (!state) return;
    
    /* Refresh all windows */
    wrefresh(state->ui.main_win);
    wrefresh(state->ui.status_win);
    wrefresh(state->ui.command_win);
}

/**
 * Clear display
 */
void ui_clear(EditorState *state) {
    if (!state) return;
    
    /* Clear all windows */
    werase(state->ui.main_win);
    werase(state->ui.status_win);
    werase(state->ui.command_win);
}

/**
 * Get keyboard input
 */
int ui_get_key(EditorState *state) {
    if (!state) return ERR;
    
    /* Check for terminal resize */
    int ch = getch();
    if (ch == KEY_RESIZE) {
        ui_resize(state);
        return ERR;
    }
    
    return ch;
}