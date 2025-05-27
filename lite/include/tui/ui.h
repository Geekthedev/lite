/**
 * ui.h - Terminal UI handling for LITE editor
 */

#ifndef LITE_UI_H
#define LITE_UI_H

#include <ncurses.h>

/* Forward declarations */
struct EditorState;

/* UI state */
typedef struct UIState {
    WINDOW *main_win;
    WINDOW *status_win;
    WINDOW *command_win;
    int term_width;
    int term_height;
    int editor_height;
} UIState;

/* UI functions */
int ui_init(struct EditorState *state);
void ui_free(struct EditorState *state);
void ui_resize(struct EditorState *state);
void ui_render_buffer(struct EditorState *state);
void ui_render_status_line(struct EditorState *state);
void ui_render_command_line(struct EditorState *state);
void ui_render_message(struct EditorState *state);
void ui_refresh(struct EditorState *state);
void ui_clear(struct EditorState *state);
int ui_get_key(struct EditorState *state);

#endif /* LITE_UI_H */