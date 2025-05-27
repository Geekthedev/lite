/**
 * command.h - Command processing for LITE editor
 */

#ifndef LITE_COMMAND_H
#define LITE_COMMAND_H

struct EditorState;

/* Command callback function type */
typedef int (*CommandFunc)(struct EditorState *state, int argc, char **argv);

/* Command structure */
typedef struct Command {
    const char *name;
    const char *help;
    CommandFunc func;
} Command;

/* Command functions */
int command_init(void);
int command_register(const char *name, const char *help, CommandFunc func);
int command_execute(struct EditorState *state, const char *command_line);
void command_show_help(struct EditorState *state, const char *command_name);

/* Built-in commands */
int command_open(struct EditorState *state, int argc, char **argv);
int command_write(struct EditorState *state, int argc, char **argv);
int command_quit(struct EditorState *state, int argc, char **argv);
int command_tab(struct EditorState *state, int argc, char **argv);
int command_theme(struct EditorState *state, int argc, char **argv);
int command_help(struct EditorState *state, int argc, char **argv);

#endif /* LITE_COMMAND_H */