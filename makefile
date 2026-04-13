CC := gcc

CFLAGS := -Wall -Wextra -O2 \
	$(shell pkg-config --cflags libcjson ncurses)

LDFLAGS := $(shell pkg-config --libs libcjson ncurses)

SRC := src/main.c src/ui.c src/utils.c src/notes.c
TARGET := notewrapper

.PHONY: all run clean

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
