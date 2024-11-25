# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Source file and output executable
SRC = a8.c
OUT = a8

# Default target
all: $(OUT)

# Build the executable
$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Clean up build artifacts
clean:
	rm -f $(OUT)
