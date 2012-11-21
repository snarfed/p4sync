##
## Base Makefile for P4. (requires GNU make!)
##

include Makefile.in

# SRCS is a :=, ie "simple" variable, so that it only gets expanded once
HDRS := $(wildcard *.h)
SRCS := $(filter-out %_test.cpp %_stub.cpp %generated.cpp, $(wildcard *.cpp))
TESTS := $(wildcard *_test.cpp)
OBJS = $(SRCS:.cpp=.o)
LIBS = http-tiny/http_lib.o libmsntp/libmsntp.a libwebserver/libwebserver.a

default all install:: $(OBJS)
	$(MAKE) -C libwebserver
	$(MAKE) -C libmsntp
	$(MAKE) -C http-tiny

allclean : clean
	$(MAKE) -C libwebserver clean
	$(MAKE) -C libmsntp clean
	$(MAKE) -C http-tiny clean
	$(MAKE) -C xmms clean
	$(MAKE) -C zinf clean

clean::
	rm -f doc/html/* server_stub test.bin tests.generated.cpp

dist : allclean
	ln -s . $(PKGNAME)
	tar cjhf $(PKGNAME).tar.bz2 --exclude $(PKGNAME).tar.bz2 --exclude .svn \
	  --exclude $(PKGNAME)/$(PKGNAME) --exclude p4sync.lsm $(PKGNAME)
	rm $(PKGNAME)


# XMMS plugin
xmms/libp4sync.so : $(OBJS) all $(LIBS)
	$(MAKE) -C xmms
	$(CPP) $(LDFLAGS) $(OBJS) $(LIBS) xmms/libp4sync.a -shared -o $@

# zinf plugin
zinf/p4sync.ui : $(OBJS) all $(LIBS)
	$(MAKE) -C zinf
	$(CPP) $(LDFLAGS) $(OBJS) $(LIBS) zinf/p4sync.a -shared -o $@

# p4sync-enabled mpc
mpc/mpc : $(OBJS) all $(LIBS)
	$(MAKE) -C mpc
	$(CPP) $(LDFLAGS) -lpthread $(OBJS) $(LIBS) mpc/*.o -o $@


# Unit tests
tests.generated.cpp : $(TESTS) $(HDRS)
	./cxxtest/cxxtestgen.pl --runner=StdioPrinter -o $@ $(TESTS)

server_stub: all $(OBJS) server_stub.o
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) server_stub.o

test.bin : tests.generated.o 
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) tests.generated.o 

test : $(OBJS) all test.bin server_stub
	rm -f p4sync.cfg
	pkill server_stub || true
	./server_stub >& /dev/null &
	echo waiting a second to let server_stub start up...
	sleep 1s
	./test.bin
	pkill server_stub
	rm -f p4sync.cfg


# when ./test.bin seg faults, make will want to delete it since it thinks it got
# interrupted while building. however, we want to keep it around for debugging.
# hence .PRECIOUS. it's a special target; make won't delete any of its deps.
.PRECIOUS : test.bin


# Doxygen stuff
doc : FORCE
	if ! which doxygen >& /dev/null; \
	 then echo 'doxygen not installed; not generating documentation!'; \
	 else VERSION=$(VERSION) doxygen Doxyfile; \
	fi

FORCE :  # needed since make can't tell whether or not doc is "up to date"

install:: doc
	install -bd -m 755 doc/html $(DOCDIR)
