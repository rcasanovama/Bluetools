# Makefile - Raúl Casanova Marqués

CC = gcc
LD = gcc

CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g
CFLAGS += -O -D_GNU_SOURCE

CLIBS = -lbluetooth

OBJ_DIR = obj
BIN_DIR = bin

OBJ_DIRTYTOOTH = $(OBJ_DIR)/src/pbap.o $(OBJ_DIR)/src/obex-utils.o $(OBJ_DIR)/src/obex.o $(OBJ_DIR)/src/rfcomm.o $(OBJ_DIR)/main.o
BIN_DIRTYTOOTH = $(BIN_DIR)/dirtytooth


all: test_dirtytooth build_dirtytooth

clean: clean_dirtytooth

test_dirtytooth:
	test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

build_dirtytooth: test_dirtytooth

	# Build obj files
	$(CC) $(CFLAGS) -c src/pbap.c -o $(OBJ_DIR)/src/pbap.o
	$(CC) $(CFLAGS) -c src/obex-utils.c -o $(OBJ_DIR)/src/obex-utils.o
	$(CC) $(CFLAGS) -c src/obex.c -o $(OBJ_DIR)/src/obex.o
	$(CC) $(CFLAGS) -c src/rfcomm.c -o $(OBJ_DIR)/src/rfcomm.o
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

	# Link and generate dirtytooth program
	$(LD) -o $(BIN_DIRTYTOOTH) $(OBJ_DIRTYTOOTH) $(CLIBS)

clean_dirtytooth:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
