# Makefile for a C program with custom compilation flags

# Compiler and compiler flags
CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors 

# Source files and the output executable name
SRCS = symnmf.c matrix.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = symnmf

# Default target
all: $(EXECUTABLE)

# Compile the program
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

# Compile individual source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@