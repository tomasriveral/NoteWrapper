# Makefile for the Notes Vault project

# Compiler
CC = gcc

# Source files
SRC = startup.c

# Output binary
TARGET = nvimnotes

# Compiler flags
CFLAGS = -Wall -O2 `pkg-config --cflags libcjson ncurses`

# Linker flags
LDFLAGS = `pkg-config --libs libcjson ncurses`

# Debug mode
DEBUG ?= 0
ifeq ($(DEBUG),1)
    CFLAGS += -g
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
