# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


LIBS += -lm

PROG := labyrinth

SRCS += \
	coordinate.cpp \
	grid.cpp \
	labyrinth.cpp \
	maze.cpp \
	router.cpp \
	$(LIB)/list-htm-v2.cpp \
	$(LIB)/mt19937ar.c \
	$(LIB)/pair-htm-v2.cpp \
	$(LIB)/queue-htm-v2.cpp \
	$(LIB)/random.c \
	$(LIB)/vector.c \
#
OBJS := ${SRCS:.c=.o}

CFLAGS += -DUSE_EARLY_RELEASE


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
