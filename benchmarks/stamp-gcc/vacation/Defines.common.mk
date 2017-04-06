# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CFLAGS += -DLIST_NO_DUPLICATES
CFLAGS += -DMAP_USE_RBTREE

PROG := vacation

SRCS += \
	client.cpp \
	customer-htm.cpp \
	manager-htm.cpp \
	reservation-htm.cpp \
	vacation.cpp \
	$(LIB)/list-htm-v2.cpp \
	$(LIB)/pair-htm-v2.cpp \
	$(LIB)/mt19937ar.c \
	$(LIB)/random.c \
	$(LIB)/rbtree-htm-v2.cpp \
#
OBJS := ${SRCS:.c=.o}


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
