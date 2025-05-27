/**
 * highlight.c - Syntax highlighting for LITE editor
 */

#include "lite.h"
#include "syntax/highlight.h"
#include "utils/log.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Color pairs for token types */
static int token_colors[TOK_COUNT];

/* Keywords for supported languages */
static const char *c_keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
    NULL
};

static const char *c_types[] = {
    "int", "char", "float", "double", "void", "unsigned", "signed",
    "short", "long", "struct", "union", "enum", "typedef", "static",
    NULL
};

static const char *js_keywords[] = {
    "break", "case", "catch", "class", "const", "continue", "debugger",
    "default", "delete", "do", "else", "export", "extends", "finally",
    "for", "function", "if", "import", "in", "instanceof", "new", "return",
    "super", "switch", "this", "throw", "try", "typeof", "var", "void",
    "while", "with", "yield", "let", "await", "async",
    NULL
};

static const char *java_keywords[] = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch",
    "char", "class", "const", "continue", "default", "do", "double",
    "else", "enum", "extends", "final", "finally", "float", "for",
    "goto", "if", "implements", "import", "instanceof", "int", "interface",
    "long", "native", "new", "package", "private", "protected", "public",
    "return", "short", "static", "strictfp", "super", "switch", "synchronized",
    "this", "throw", "throws", "transient", "try", "void", "volatile", "while",
    NULL
};

/**
 * Check if a string is in a keyword list
 */
static bool is_keyword(const char *word, const char **keywords) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Initialize syntax highlighting
 */
void highlight_init(void) {
    /* Initialize color pairs for token types */
    token_colors[TOK_DEFAULT] = 1;      /* White on black */
    token_colors[TOK_KEYWORD] = 2;      /* Green on black */
    token_colors[TOK_TYPE] = 3;         /* Cyan on black */
    token_colors[TOK_STRING] = 4;       /* Yellow on black */
    token_colors[TOK_COMMENT] = 5;      /* Blue on black */
    token_colors[TOK_NUMBER] = 6;       /* Magenta on black */
    token_colors[TOK_IDENTIFIER] = 7;   /* White on black */
    token_colors[TOK_PREPROCESSOR] = 8; /* Red on black */
    token_colors[TOK_OPERATOR] = 9;     /* Green on black */
}

/**
 * Set color for a token type
 */
void highlight_set_color(TokenType type, short fg, short bg) {
    if (type >= 0 && type < TOK_COUNT) {
        /* Find or create color pair */
        int pair = type + 1;
        init_pair(pair, fg, bg);
        token_colors[type] = pair;
    }
}

/**
 * Detect language from filename
 */
LanguageId highlight_detect_language(const char *filename) {
    if (!filename) return LANG_UNKNOWN;
    
    /* Find extension */
    const char *ext = strrchr(filename, '.');
    if (!ext) return LANG_UNKNOWN;
    
    /* Match extension to language */
    if (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0) {
        return LANG_C;
    } else if (strcmp(ext, ".js") == 0 || strcmp(ext, ".jsx") == 0) {
        return LANG_JS;
    } else if (strcmp(ext, ".java") == 0) {
        return LANG_JAVA;
    }
    
    return LANG_UNKNOWN;
}

/**
 * Highlight a line of text
 */
void highlight_line(WINDOW *win, const char *line, int line_num, LanguageId lang) {
    if (!win || !line) return;
    
    /* For now, just display the line */
    mvwaddstr(win, line_num, 0, line);
    
    /* TODO: Implement proper syntax highlighting */
}