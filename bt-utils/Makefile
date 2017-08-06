# Makefile - Raúl Casanova Marqués

CC = gcc
LD = gcc

CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g
CFLAGS += -O -D_GNU_SOURCE

CLIBS = -lbluetooth

OBJ_DIR = obj
BIN_DIR = bin

OBJ_BT_UTILS = $(OBJ_DIR)/src/bdaddr.o $(OBJ_DIR)/src/bt-adapter.o $(OBJ_DIR)/src/bt-utils.o $(OBJ_DIR)/main.o
BIN_BT_UTILS = $(BIN_DIR)/bt-utils


all: test_bt_utils build_bt_utils

clean: clean_bt_utils

test_bt_utils:
	test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

build_bt_utils: test_bt_utils

	# Build obj files
	$(CC) $(CFLAGS) -c src/bdaddr.c -o $(OBJ_DIR)/src/bdaddr.o
	$(CC) $(CFLAGS) -c src/bt-adapter.c -o $(OBJ_DIR)/src/bt-adapter.o
	$(CC) $(CFLAGS) -c src/bt-utils.c -o $(OBJ_DIR)/src/bt-utils.o
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

	# Link and generate bt-utils program
	$(LD) -o $(BIN_BT_UTILS) $(OBJ_BT_UTILS) $(CLIBS)

clean_bt_utils:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
