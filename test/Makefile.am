AM_CFLAGS = -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Wmain -pedantic-errors -pedantic -Wshadow -Wextra -Wall -g -O -D_GNU_SOURCE

# programs.
bin_PROGRAMS = test-adapter test-scanner test-utils

# test-adapter configurations.
test_adapter_SOURCES = test-adapter.c \
                       ../src/bt-types.h \
                       ../src/bt-adapter.c \
                       ../src/bt-adapter.h \
                       ../lib/bdaddr.c \
                       ../lib/oui.c \
                       ../lib/oui.h

test_adapter_CFLAGS = $(AM_CFLAGS)
test_adapter_LDFLAGS = -lbluetooth

# test-scanner configurations.
test_scanner_SOURCES = test-scanner.c \
                       ../src/bt-types.h \
                       ../src/bt-scanner.c \
                       ../src/bt-scanner.h \
                       ../src/bt-adapter.c \
                       ../src/bt-adapter.h \
                       ../lib/bdaddr.c \
                       ../lib/oui.c \
                       ../lib/oui.h

test_scanner_CFLAGS = $(AM_CFLAGS)
test_scanner_LDFLAGS = -lbluetooth

# test-utils configurations.
test_utils_SOURCES = test-utils.c \
                       ../src/bt-types.h \
                       ../src/bt-utils.c \
                       ../src/bt-utils.h \
                       ../src/bt-adapter.c \
                       ../src/bt-adapter.h \
                       ../lib/bdaddr.c \
                       ../lib/oui.c \
                       ../lib/oui.h

test_utils_CFLAGS = $(AM_CFLAGS)
test_utils_LDFLAGS = -lbluetooth
