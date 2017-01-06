CC = clang
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O3

TARGET = patdown
SRCS   = errors.c links.c main.c markdown.c parsers.c strings.c
OBJS  := $(SRCS:%.c=%.o)

all: $(TARGET)
	
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

debug: CFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
debug: $(TARGET)

errors.o: errors.c errors.h
links.o: links.c errors.h links.h
main.o: main.c errors.h markdown.h strings.h
markdown.o: markdown.c errors.h links.h markdown.h strings.h
strings.o: strings.c errors.h strings.h

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
