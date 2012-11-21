##
## Makefile for the p4sync zinf plugin
##

include ../Makefile.in

# SRCS is a :=, ie "simple" variable, so that it only gets expanded once
SRCS := $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
CPPFLAGS := $(CPPFLAGS) -DENABLE_NLS  # for include/gettext.h

default all:: p4sync.a

clean::
	rm -f p4sync.ui

p4sync.a : $(OBJS)
	ar -r $@ $(OBJS)
	ranlib $@

