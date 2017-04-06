# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CFLAGS += -DLIST_NO_DUPLICATES
CFLAGS += -DCHUNK_STEP1=12

PROG := genome

SRCS += \
	gene.cpp \
	genome.cpp \
	segments.cpp \
	sequencer.cpp \
	table-htm.cpp \
	$(LIB)/bitmap.c \
	$(LIB)/hash.c \
	$(LIB)/hashtable-htm.cpp \
	$(LIB)/pair-htm.cpp \
	$(LIB)/random.c \
	$(LIB)/list-htm.cpp \
	$(LIB)/mt19937ar.c \
	$(LIB)/vector.c \
#
OBJS := ${SRCS:.c=.o}


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
