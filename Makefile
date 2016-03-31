CC=clang
TARGET=patdown
TEST_TARGET=test

BUILD_OBJS=main.o
TEST_OBJS=tests/tests.o tests/tests_parsing.o
GEN_OBJS=arguments.o files.o errors.o markdown.o parsers.o utilities.o
TEST_FILES=test.md

build: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)
	
test: $(TEST_OBJS) $(GEN_OBJS)
	$(CC) -o $(TEST_TARGET) $(TEST_OBJS) $(GEN_OBJS)
	
debug: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -g -O0 -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)
	
arguments.o: arguments.c arguments.h errors.h
	$(CC) -c arguments.c

files.o: files.c files.h arguments.h errors.h
	$(CC) -c files.c
	
errors.o: errors.c errors.h
	$(CC) -c errors.c
	
markdown.o: markdown.c markdown.h files.h parsers.h utilities.h
	$(CC) -c markdown.c
	
parsers.o: parsers.c parsers.h markdown.h utilities.h
	$(CC) -c parsers.c
	
utilities.o: utilities.c utilities.h
	$(CC) -c utilities.c

main.o: main.c arguments.h files.h errors.h markdown.h
	$(CC) -c main.c

tests_parsing.o: tests/tests_parsing.c tests/tests_parsing.h markdown.h parsers.h
	$(CC) -c tests/tests_parsing.c

tests.o: tests/tests.c tests/tests_parsing.h
	$(CC) -c tests/tests.c

clean: 
	rm -f $(TARGET) $(TEST_TARGET) $(TEST_OBJS) $(BUILD_OBJS) $(GEN_OBJS) $(TEST_FILES)