# LITE Editor

**Lightweight Interface for Terminal Engineering**

LITE is a minimalistic terminal-based code editor designed for developers who prefer working in a terminal environment. It aims to be fast, lightweight, and highly customizable.

## Installation

### Windows

1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 terminal and run:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-pdcurses make
   ```
3. Clone and build LITE:
   ```bash
   git clone https://github.com/yourusername/lite
   cd lite
   make
   ```

### Linux

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install gcc make libncurses5-dev
git clone https://github.com/yourusername/lite
cd lite
make
```

#### Fedora/RHEL
```bash
sudo dnf install gcc make ncurses-devel
git clone https://github.com/yourusername/lite
cd lite
make
```

#### Arch Linux
```bash
sudo pacman -S gcc make ncurses
git clone https://github.com/yourusername/lite
cd lite
make
```

## Features

- Terminal-based UI using ncurses/pdcurses
- Vim-style `:` command interface
- Multiple buffer support with tabs
- Syntax highlighting for C, JavaScript, and Java
- Customizable configuration via `.lightrc` file
- Basic file operations (open, save, close)

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
├── src/                  # Source files
├── include/              # Header files
├── themes/               # User themes
├── .lightrc             # Editor config
├── Makefile
└── README.md
```

## License

This project is open source.
