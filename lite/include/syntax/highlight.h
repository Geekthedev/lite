/**
 * highlight.h - Syntax highlighting for LITE editor
 */

#ifndef LITE_HIGHLIGHT_H
#define LITE_HIGHLIGHT_H

#include <ncurses.h>
#include "../core/buffer.h"

/* Token types */
typedef enum {
    TOK_DEFAULT,
    TOK_KEYWORD,
    TOK_TYPE,
    TOK_STRING,
    TOK_COMMENT,
    TOK_NUMBER,
    TOK_IDENTIFIER,
    TOK_PREPROCESSOR,
    TOK_OPERATOR,
    TOK_COUNT
} TokenType;

/* Language IDs */
typedef enum {
    LANG_UNKNOWN,
    LANG_C,
    LANG_JS,
    LANG_JAVA,
    LANG_COUNT
} LanguageId;

/* Highlight functions */
void highlight_init(void);
void highlight_set_color(TokenType type, short fg, short bg);
LanguageId highlight_detect_language(const char *filename);
void highlight_line(WINDOW *win, const char *line, int line_num, LanguageId lang);

#endif /* LITE_HIGHLIGHT_H */