##
## Makefile for the p4sync xmms plugin
##

include ../Makefile.in

# SRCS is a :=, ie "simple" variable, so that it only gets expanded once
SRCS := $(wildcard *.cpp *.c)
OBJS = $(SRCS:.cpp=.o) $(SRCS:.c=.o)
CPPFLAGS := $(CPPFLAGS) -I/usr/include/glib-1.2 -I/usr/lib/glib/include \
	-I/usr/include/gtk-1.2
CFLAGS = $(CPPFLAGS)

default all:: libp4sync.a

libp4sync.a : $(OBJS)
	ar -r $@ $(OBJS)
	ranlib $@

