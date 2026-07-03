# Compiler and flags
CC       := gcc
CFLAGS   := -Wall -Wextra -Werror -std=c11
TARGET   := bin/dearsize

# Installation paths
PREFIX   ?= /usr/local
BINDIR   := $(PREFIX)/bin

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

install: all
	@if [ ! -w "$(BINDIR)" ]; then \
		echo "Permission error: You must use 'sudo make install'"; \
		exit 1; \
	fi
	@echo "Installing $(TARGET) to $(BINDIR)..."
	@mkdir -p $(BINDIR)
	cp $(TARGET) $(BINDIR)/dearsize
	chmod 755 $(BINDIR)/dearsize
	@echo "Installation successful!"

uninstall:
	@if [ ! -w "$(BINDIR)" ]; then \
		echo "Permission error: You must use 'sudo make uninstall'"; \
		exit 1; \
	fi
	@echo "Removing dear_size from $(BINDIR)..."
	rm -f $(BINDIR)/dearsize
	@echo "Uninstalled successfully!"

.PHONY: all clean install uninstall
