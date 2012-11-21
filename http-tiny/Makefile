##
## Makefile for http-tiny, a library used in p4sync
##

include ../Makefile.in

# SRCS is a :=, ie "simple" variable, so that it only gets expanded once
SRCS := $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

default all:: http_lib.o
