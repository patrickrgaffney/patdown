CC=clang
TARGET=patdown
TEST_TARGET=test

BUILD_OBJS=main.o
GEN_OBJS=errors.o markdown.o parsers.o strings.o files.o
TEST_OBJS=parser_tests.o

build: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)

tests: $(TEST_OBJS) $(GEN_OBJS)
	$(CC) -o $(TEST_TARGET) $(TEST_OBJS) $(GEN_OBJS)
	
debug: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -g -O0 -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)

main.o: main.c markdown.h parsers.h files.h arguments.h strings.h
	$(CC) -c main.c
	
errors.o: errors.c errors.h
	$(CC) -c errors.c

markdown.o: markdown.c markdown.h strings.h errors.h parsers.h files.h
	$(CC) -c markdown.c
	
parsers.o: parsers.c parsers.h markdown.h files.h strings.h
	$(CC) -c parsers.c
	
strings.o: strings.c strings.h errors.h
	$(CC) -c strings.c
	
files.o: files.c files.h strings.h
	$(CC) -c files.c

parser_tests.o: parser_tests.c markdown.h parsers.h
	$(CC) -c parser_tests.c

clean: 
	rm -f $(TARGET) $(BUILD_OBJS) $(GEN_OBJS) $(TEST_OBJS) $(TEST_TARGET)