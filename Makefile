CC=clang
TARGET=patdown
TEST_TARGET=test

BUILD_OBJS=main.o
TEST_OBJS=tests.o
GEN_OBJS=arguments.o files.o errors.o
TEST_FILES=test.md

build: $(BUILD_OBJS) $(GEN_OBJS)
	$(CC) -o $(TARGET) $(BUILD_OBJS) $(GEN_OBJS)
	
test: $(TEST_OBJS) $(GEN_OBJS)
	$(CC) -o $(TEST_TARGET) $(TEST_OBJS) $(GEN_OBJS)
	
arguments.o: arguments.c arguments.h errors.h
	$(CC) -c arguments.c

files.o: files.c files.h arguments.h errors.h
	$(CC) -c files.c
	
errors.o: errors.c errors.h
	$(CC) -c errors.c

main.o: main.c arguments.h files.h errors.h
	$(CC) -c main.c

tests.o: tests.c arguments.h files.h
	$(CC) -c tests.c

clean: 
	rm -f $(TARGET) $(TEST_TARGET) $(TEST_OBJS) $(BUILD_OBJS) $(GEN_OBJS) $(TEST_FILES)