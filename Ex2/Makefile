CC=g++
RANLIB=ranlib

LIBSRC= uthreads.cpp Thread.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11 # $(INCS)
LOADLIBES = -L./ 

TARGETS = libuthreads.a

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS = $(LIBSRC) Thread.h Makefile README

all: uthreads

test.o: uthreads.h
	$(CC) $(CFLAGS)  test.cpp -o test.o

Thread.o: Thread.cpp Thread.h
	$(CC) $(CFLAGS) Thread.cpp -o Thread.o

uthreads.o: uthreads.cpp uthreads.h
	$(CC) $(CFLAGS) uthreads.cpp -o uthreads.o

main: uthreads.o Thread.o test.o
	$(CC) -o main uthreads.o Thread.o test.o

uthreads: uthreads.o Thread.o
	ar rcs libuthreads.a uthreads.o Thread.o

clean:
	$(RM) $(TARGETS) $(LIBOBJ) $(TARNAME)


tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
