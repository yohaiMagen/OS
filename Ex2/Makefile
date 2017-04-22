CC=g++
RANLIB=ranlib

LIBSRC= uthreads.cpp thread.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11 # $(INCS)
LOADLIBES = -L./ 

TARGETS = libuthreads.a

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS = $(LIBSRC) thread.h Makefile README

all: uthreads

test.o: uthreads.h
	$(CC) $(CFLAGS)  test.cpp -o test.o

thread.o: thread.cpp thread.h
	$(CC) $(CFLAGS) thread.cpp -o thread.o

uthreads.o: uthreads.cpp uthreads.h
	$(CC) $(CFLAGS) uthreads.cpp -o uthreads.o

main: uthreads.o thread.o test.o
	$(CC) -o main uthreads.o thread.o test.o

uthreads: uthreads.o thread.o
	ar rcs libuthreads.a uthreads.o thread.o

clean:
	$(RM) $(TARGETS) $(LIBOBJ) $(TARNAME) *~ *core


tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
