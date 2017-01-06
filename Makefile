CC=clang
TARGET=patdown
UNIX_TARGET=a.out
CC_FLAGS= -std=c99 -Wall -Wextra -pedantic
CCX=clang $(CC_FLAGS)

BUILD_OBJS=main.o
GEN_OBJS=errors.o strings.o files.o markdown.o parsers.o links.o

build: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) $(C_FLAGS) -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)

debug: C_FLAGS += -g -fsanitize=address -fno-omit-frame-pointer
debug: build
	
valgrind: C_FLAGS += -g
valgrind: build

main.o: main.c errors.h files.h markdown.h parsers.h
	$(CCX) -c $(C_FLAGS) main.c
	
errors.o: errors.c errors.h
	$(CCX) -c $(C_FLAGS) errors.c
	
strings.o: strings.c errors.h strings.h
	$(CCX) -c $(C_FLAGS) strings.c
	
files.o: files.c errors.h files.h strings.h
	$(CCX) -c $(C_FLAGS) files.c

markdown.o: markdown.c markdown.h errors.h strings.h
	$(CCX) -c $(C_FLAGS) markdown.c

parsers.o: parsers.c parsers.h markdown.h strings.h
	$(CCX) -c $(C_FLAGS) parsers.c

links.o: links.c errors.h links.h
	$(CCX) -c $(C_FLAGS) links.c

clean: 
	rm -f $(TARGET) $(BUILD_OBJS) $(GEN_OBJS) $(UNIX_TARGET) 