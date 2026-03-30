CC = gcc
SRC = startup.c
TARGET = nvimnotes
CFLAGS = -Wall -O2 `pkg-config --cflags libcjson ncurses`
LDFLAGS = `pkg-config --libs libcjson ncurses`
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
