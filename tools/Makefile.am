AM_CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g -O -D_GNU_SOURCE

# programs.
bin_PROGRAMS = dirtytooth agent

# dirtytooth configurations.
dirtytooth_SOURCES = dirtytooth.c ../src/rfcomm.c \
                                  ../src/rfcomm.h \
                                  ../src/obex-types.h \
                                  ../src/obex-utils.c \
                                  ../src/obex-utils.h \
                                  ../src/obex.c \
                                  ../src/obex.h \
                                  ../src/pbap-types.h \
                                  ../src/pbap.c \
                                  ../src/pbap.h

dirtytooth_CFLAGS = $(AM_CFLAGS)
dirtytooth_LDFLAGS = -lbluetooth

# agent configurations.
agent_SOURCES = agent.c
agent_CFLAGS = $(AM_CFLAGS)
agent_LDFLAGS = -ldbus-1
