# LITE Editor Makefile

# Detect OS
ifeq ($(OS),Windows_NT)
    CC = gcc
    TARGET_EXT = .exe
    RM = del /Q
    MKDIR = mkdir
    RMDIR = rmdir /S /Q
    LDFLAGS = -lpdcurses
else
    CC = gcc
    TARGET_EXT =
    RM = rm -f
    MKDIR = mkdir -p
    RMDIR = rm -rf
    LDFLAGS = -lncurses
endif

CFLAGS = -Wall -Wextra -g -I./include
SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist
BIN_DIR = .

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) \
            $(wildcard $(SRC_DIR)/core/*.c) \
            $(wildcard $(SRC_DIR)/tui/*.c) \
            $(wildcard $(SRC_DIR)/fs/*.c) \
            $(wildcard $(SRC_DIR)/utils/*.c) \
            $(wildcard $(SRC_DIR)/syntax/*.c) \
            $(wildcard $(SRC_DIR)/plugins/*.c) \
            $(wildcard $(SRC_DIR)/gitlite/*.c)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Ensure build directories exist
BUILD_DIRS = $(BUILD_DIR) \
             $(BUILD_DIR)/core \
             $(BUILD_DIR)/tui \
             $(BUILD_DIR)/fs \
             $(BUILD_DIR)/utils \
             $(BUILD_DIR)/syntax \
             $(BUILD_DIR)/plugins \
             $(BUILD_DIR)/gitlite

# Main target
TARGET = $(BIN_DIR)/lite$(TARGET_EXT)

.PHONY: all clean dirs dist

all: dirs $(TARGET)

dirs:
	$(MKDIR) $(BUILD_DIRS)
	$(MKDIR) $(DIST_DIR)

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RMDIR) $(BUILD_DIR)
	$(RMDIR) $(DIST_DIR)
	$(RM) $(TARGET)

dist: all
	$(MKDIR) $(DIST_DIR)
	$(MKDIR) $(DIST_DIR)/themes
	cp $(TARGET) $(DIST_DIR)/
	cp README.md $(DIST_DIR)/
	cp .lightrc $(DIST_DIR)/
	-cp -r themes/* $(DIST_DIR)/themes/
