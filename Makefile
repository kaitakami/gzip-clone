CC = gcc
CFLAGS = -Wall -Wextra -I./include -pthread $(shell pkg-config --cflags check)
LDFLAGS = -lz -pthread $(shell pkg-config --libs check)
SRC = src/main.c src/cli.c src/config.c src/help.c src/version.c src/compress.c src/decompress.c src/directory.c src/threadpool.c src/logging.c src/file_utils.c src/progress.c
OBJ = $(SRC:.c=.o)
TARGET = gzip-clone

TEST_SRC = tests/test_main.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = run_tests

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) tests/*.o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(filter-out src/main.o, $(OBJ))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean test
