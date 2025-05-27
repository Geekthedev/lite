/**
 * lite.h - Main header file for LITE editor
 * 
 * LITE (Lightweight Interface for Terminal Engineering)
 * A minimalist terminal-based code editor
 */

#ifndef LITE_H
#define LITE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

/* Version information */
#define LITE_VERSION "0.1.0"
#define LITE_AUTHOR "LITE Team"

/* Configuration */
#define LITE_CONFIG_FILE ".lightrc"
#define LITE_MAX_BUFFERS 10
#define LITE_MAX_LINE_LENGTH 1024
#define LITE_TAB_WIDTH 4

/* Error codes */
#define LITE_OK 0
#define LITE_ERROR -1
#define LITE_ERROR_FILE_NOT_FOUND -2
#define LITE_ERROR_BUFFER_FULL -3

/* Mode definitions */
typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND,
    MODE_VISUAL
} EditorMode;

/* Forward declarations */
struct Buffer;
struct EditorConfig;
struct EditorState;

/* Include component headers */
#include "core/buffer.h"
#include "core/editor.h"
#include "tui/ui.h"
#include "fs/file.h"
#include "utils/log.h"
#include "syntax/highlight.h"
#include "core/command.h"

#endif /* LITE_H */