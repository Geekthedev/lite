# LITE Editor

**Lightweight Interface for Terminal Engineering**

LITE is a minimalistic terminal-based code editor designed for developers who prefer working in a terminal environment. It aims to be fast, lightweight, and highly customizable.

## Features

- Terminal-based UI using ncurses
- Vim-style `:` command interface
- Multiple buffer support with tabs
- Syntax highlighting for C, JavaScript, and Java
- Customizable configuration via `.lightrc` file
- Basic file operations (open, save, close)

## Building from Source

```bash
make
```

## Usage

```bash
./lite [file...]
```

### Commands

- `:open <file>` - Open a file for editing
- `:write` - Save the current file
- `:quit` or `:q` - Quit LITE (`:q!` to force quit)
- `:tab new` - Create a new buffer
- `:tab <id>` - Switch to buffer by ID
- `:theme load <name>` - Load a theme
- `:help [command]` - Show help

### Keybindings

- Normal mode: `h`, `j`, `k`, `l` for navigation
- `i` - Enter insert mode
- `ESC` - Return to normal mode
- `:` - Enter command mode

## Project Structure

```
lite/
├── src/
│   ├── main.c               # App entry point
│   ├── core/                # Buffer, input, editor logic
│   ├── tui/                 # Terminal UI rendering
│   ├── fs/                  # File I/O & config
│   ├── syntax/              # Highlighting and lexers
│   ├── plugins/             # JS engine, loader, plugin API
│   ├── gitlite/             # Minimal git system
│   └── utils/               # Logging, strings, helpers
├── include/                 # Header files
├── themes/                 # User themes
├── .lightrc                # Editor config
├── Makefile
└── README.md
```

## License

This project is open source.

### Author
Anointed Joseph