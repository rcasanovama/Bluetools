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
test_adapter_LDADD = -lbluetooth

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
test_scanner_LDADD = -lbluetooth

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
test_utils_LDADD = -lbluetooth
