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
dirtytooth_LDADD = -lbluetooth

# agent configurations.
agent_SOURCES = agent.c
agent_LDADD = -ldbus-1
