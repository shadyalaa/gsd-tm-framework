# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


PROG := intruder

SRCS += \
	decoder-htm-v2.cpp \
	detector.cpp \
	dictionary.cpp \
	intruder.cpp \
	packet.cpp \
	preprocessor.cpp \
	stream-htm-v2.cpp \
	$(LIB)/list-htm-v2.cpp \
	$(LIB)/mt19937ar.c \
	$(LIB)/pair-htm-v2.cpp \
	$(LIB)/queue-htm-v2.cpp \
	$(LIB)/random.c \
	$(LIB)/rbtree-htm-v2.cpp \
	$(LIB)/vector.c \
#
OBJS := ${SRCS:.c=.o}

CFLAGS += -DMAP_USE_RBTREE


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
