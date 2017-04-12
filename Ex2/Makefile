CC=g++
RANLIB=ranlib

LIBSRC=osm.c dumyFunc.c dumyFunc.h
LIBOBJ=$(LIBSRC:.c=.o)

INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11 # $(INCS)
LOADLIBES = -L./ 

OSMLIB = libosm.a
TARGETS = $(OSMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex1.tar
TARSRCS=$(LIBSRC) Makefile README

all: main


thread.o: thread.cpp thread.h
	$(CC) $(CFLAGS) thread.cpp -o thread.o

uthreads.o: uthreads.cpp uthreads.h
	$(CC) $(CFLAGS) uthreads.cpp -o uthreads.o

main: uthreads.o thread.o
	$(CC) -o main uthreads.o thread.o 

#osm: osm.o dumyFunc.o
#	ar rcs libosm.a dumyFunc.o osm.o

clean:
	$(RM) $(TARGETS) $(OSMLIB) $(OBJ) $(LIBOBJ) *~ *core


tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
