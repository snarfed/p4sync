##
## Makefile for the p4sync xmms plugin
##

include ../Makefile.in

# SRCS is a :=, ie "simple" variable, so that it only gets expanded once
SRCS := $(wildcard *.cpp *.c)
OBJS = $(SRCS:.cpp=.o) $(SRCS:.c=.o)

default all:: $(OBJS)

clean::
	rm -f mpc
