# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CFLAGS += -DLIST_NO_DUPLICATES
CFLAGS += -DMAP_USE_AVLTREE
CFLAGS += -DSET_USE_RBTREE

PROG := yada
SRCS += \
	coordinate.cpp \
	element-htm.cpp \
	mesh-htm.cpp \
	region-htm.cpp \
	yada.cpp \
	$(LIB)/avltree.c \
	$(LIB)/heap-htm.cpp \
	$(LIB)/list-htm-v2.cpp \
	$(LIB)/mt19937ar.c \
	$(LIB)/pair-htm-v2.cpp \
	$(LIB)/queue-htm-v2.cpp \
	$(LIB)/random.c \
	$(LIB)/rbtree-htm-v2.cpp \
	$(LIB)/vector.c \
#
OBJS := ${SRCS:.c=.o}


# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
