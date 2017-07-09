# Makefile - Raúl Casanova Marqués

CC = gcc
LD = gcc

CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g -ansi -std=c11
CFLAGS += -O

CLIBS = -lbluetooth

OBJ_DIR = obj
BIN_DIR = bin

OBJ_BT_SCANNER = $(OBJ_DIR)/src/bt-scanner.o $(OBJ_DIR)/main.o
BIN_BT_SCANNER = $(BIN_DIR)/bt-scanner


all: test_bt_scanner build_bt_scanner

clean: clean_bt_scanner

test_bt_scanner:
	test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

build_bt_scanner: test_bt_scanner

	# Build obj files
	$(CC) $(CFLAGS) -c src/bt-scanner.c -o $(OBJ_DIR)/src/bt-scanner.o
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

	# Link and generate bt-scanner program
	$(LD) -o $(BIN_BT_SCANNER) $(OBJ_BT_SCANNER) $(CLIBS)

clean_bt_scanner:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
