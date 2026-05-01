# OpenGTU — Makefile
CC      = gcc
CFLAGS  = -std=c11 -O2 -Wall -Wextra -Iinclude -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
LDFLAGS = -lm
SRCS    = $(wildcard src/*.c)
OBJS    = $(SRCS:src/%.c=build/%.o)
TARGET  = opengtu

debug: CFLAGS += -g -fsanitize=address -DDEBUG
debug: LDFLAGS += -fsanitize=address
debug: clean all

.PHONY: all clean run

all: build $(TARGET)

build:
	mkdir -p build

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

run: all
	./$(TARGET)

clean:
	rm -rf build $(TARGET)
