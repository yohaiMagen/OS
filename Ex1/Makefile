CC=g++
RANLIB=ranlib

LIBSRC=osm.c dumyFunc.c dumyFunc.h
LIBOBJ=$(LIBSRC:.c=.o)

INCS=-I.
CFLAGS = -Wall -g -O0 -Og -c $(INCS)
LOADLIBES = -L./ 

OSMLIB = libosm.a
TARGETS = $(OSMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex1.tar
TARSRCS=$(LIBSRC) Makefile README

all: osm


dumyFunc.o: dumyFunc.c dumyFunc.h
	$(CC) $(CFLAGS) dumyFunc.c -o dumyFunc.o

osm.o: osm.c osm.h
	$(CC) $(CFLAGS)  osm.c -o osm.o

osm: osm.o dumyFunc.o
	ar rcs libosm.a dumyFunc.o osm.o

clean:
	$(RM) $(TARGETS) $(OSMLIB) $(OBJ) $(LIBOBJ) *~ *core


tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
