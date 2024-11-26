# Variables
CC = gcc
CFLAGS = -Wall -pedantic -g -ggdb
FILES = caesar_shift.c main.c
TARGET = caesar_cipher

# Default rule to build the target
all: $(TARGET)

# Rule to compile the target
$(TARGET): $(FILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(FILES)

# Rule to clean up generated files
clean:
	rm -f $(TARGET) *.o

# Rule to run the program (optional)
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run