# Makefile - Raúl Casanova Marqués

CC = gcc
LD = gcc

CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g -ansi -std=c11
CFLAGS += -O -D_GNU_SOURCE

CLIBS = -lbluetooth

OBJ_DIR = obj
BIN_DIR = bin

OBJ_BT_ADAPTER = $(OBJ_DIR)/src/bt-adapter.o $(OBJ_DIR)/main.o
BIN_BT_ADAPTER = $(BIN_DIR)/bt-adapter


all: test_bt_adapter build_bt_adapter

clean: clean_bt_adapter

test_bt_adapter:
	test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

build_bt_adapter: test_bt_adapter

	# Build obj files
	$(CC) $(CFLAGS) -c src/bt-adapter.c -o $(OBJ_DIR)/src/bt-adapter.o
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

	# Link and generate bt-adapter program
	$(LD) -o $(BIN_BT_ADAPTER) $(OBJ_BT_ADAPTER) $(CLIBS)

clean_bt_adapter:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
