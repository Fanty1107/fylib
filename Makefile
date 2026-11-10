# Name of the final executable
TARGET = fylib

# Compiler and Flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -pedantic
LDFLAGS = 

# Build configurations
DEBUG_FLAGS   = -g3 -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Directories
SRC_DIR   = .
BUILD_DIR = build
BIN_DIR   = .

# Automatically find all .c files in SRC_DIR and map them to object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# External Libraries & Include paths (uncomment and adjust as needed)
# Example for Raylib: LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
# Example using pkg-config: LIBS = $(shell pkg-config --libs sdl3)
LIBS     = 
INCLUDES = -Iinclude

# -----------------------------------------------------------------------------
# Targets
# -----------------------------------------------------------------------------

# Default target: Debug build
all: CFLAGS += $(DEBUG_FLAGS)
all: $(BIN_DIR)/$(TARGET)

# Release target: Optimized build with debugging info stripped
release: CFLAGS += $(RELEASE_FLAGS)
release: clean $(BIN_DIR)/$(TARGET)

# Linking the final executable
$(BIN_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)
	@echo "Built target: $(TARGET)"

# Compiling source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the compiled binary
run: all
	@./$(BIN_DIR)/$(TARGET)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(TARGET)
	@echo "Cleaned build artifacts."

# Declare phony targets
.PHONY: all release run clean
