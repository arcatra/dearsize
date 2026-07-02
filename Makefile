# Compiler and flags
CC       := gcc
TCFLAGS   := -Wall -Wextra -Werror -std=c11
ARGET   := bin/dearsize

# Scan for source files and map them to object files
SRCS     := $(wildcard src/*.c)
OBJS     := $(SRCS:src/%.c=build/%.o)

# Default rule (runs when you type 'make')
all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(OBJS) -o $(TARGET)

# Compile source files into object files
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf build bin

.PHONY: all clean
