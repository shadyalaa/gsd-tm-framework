CFLAGS += -DOUTPUT_TO_STDOUT

PROG := kmeans

SRCS += \
	cluster.c \
	common.c \
	kmeans.c \
	normal.c \
	$(LIB)/mt19937ar.c \
	$(LIB)/random.c \
#
OBJS := ${SRCS:.c=.o}
