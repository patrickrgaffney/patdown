CC=clang
TARGET=patdown

BUILD_OBJS=main.o
GEN_OBJS=errors.o markdown.o parsers.o strings.o files.o

build: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)
	
debug: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -g -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)

main.o: main.c errors.h files.h markdown.h parsers.h
	$(CC) -c main.c
	
errors.o: errors.c errors.h
	$(CC) -c errors.c

markdown.o: markdown.c block_types.h errors.h markdown.h strings.h
	$(CC) -c markdown.c
	
parsers.o: parsers.c block_types.h files.h markdown.h parsers.h strings.h
	$(CC) -c parsers.c
	
strings.o: strings.c errors.h strings.h
	$(CC) -c strings.c
	
files.o: files.c errors.h files.h strings.h
	$(CC) -c files.c

clean: 
	rm -f $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)