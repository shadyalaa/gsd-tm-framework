# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================

CC       := g++
CFLAGS   += -std=c++11 -I../../../rapl-power/ -g -w -pthread -mrtm -fpermissive
CFLAGS   += -O2
CFLAGS   += -I$(LIB)
CPP      := g++
CPPFLAGS += $(CFLAGS)
LD       := g++
LIBS     += -lpthread

# Remove these files when doing clean
OUTPUT +=

LIB := ../lib


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
