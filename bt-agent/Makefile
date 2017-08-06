# Makefile - Raúl Casanova Marqués

CC = gcc
LD = gcc

CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g
CFLAGS += -O -D_GNU_SOURCE

CLIBS = -lbluetooth

OBJ_DIR = obj
BIN_DIR = bin

OBJ_BT_AGENT = $(OBJ_DIR)/src/oui.o $(OBJ_DIR)/src/bdaddr.o $(OBJ_DIR)/src/bt-adapter.o $(OBJ_DIR)/src/bt-utils.o $(OBJ_DIR)/src/bt-agent.o $(OBJ_DIR)/main.o
BIN_BT_AGENT = $(BIN_DIR)/bt-agent


all: test_bt_agent build_bt_agent

clean: clean_bt_agent

test_bt_agent:
	test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

build_bt_agent: test_bt_agent

	# Build obj files
	$(CC) $(CFLAGS) -c src/oui.c -o $(OBJ_DIR)/src/oui.o
	$(CC) $(CFLAGS) -c src/bdaddr.c -o $(OBJ_DIR)/src/bdaddr.o
	$(CC) $(CFLAGS) -c src/bt-adapter.c -o $(OBJ_DIR)/src/bt-adapter.o
	$(CC) $(CFLAGS) -c src/bt-utils.c -o $(OBJ_DIR)/src/bt-utils.o
	$(CC) $(CFLAGS) -c src/bt-agent.c -o $(OBJ_DIR)/src/bt-agent.o
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

	# Link and generate bt-agent program
	$(LD) -o $(BIN_BT_AGENT) $(OBJ_BT_AGENT) $(CLIBS)

clean_bt_agent:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
