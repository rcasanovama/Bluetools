# Makefile - Raúl Casanova Marqués

CC = gcc
LD = gcc

CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g -ansi -std=c11
CFLAGS += -O

CLIBS = -lbluetooth

OBJ_DIR = obj
BIN_DIR = bin

OBJ_BLUESCAN = $(OBJ_DIR)/src/bluescan.o $(OBJ_DIR)/main.o
BIN_BLUESCAN = $(BIN_DIR)/bluescan


all: test_bluescan build_bluescan

clean: clean_bluescan

test_bluescan:
	test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

build_bluescan: test_bluescan

	# Build obj files
	$(CC) $(CFLAGS) -c src/bluescan.c -o $(OBJ_DIR)/src/bluescan.o
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

	# Link and generate bluescan program
	$(LD) -o $(BIN_BLUESCAN) $(OBJ_BLUESCAN) $(CLIBS)

clean_bluescan:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
