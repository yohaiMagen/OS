CC=g++
RANLIB=ranlib

LIBSRC= uthreads.cpp Thread.cpp


INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11 -pthread 
LOADLIBES = -L./ 

TARGETS = apReduceFramework.a MapReduceFramework.o ThreadToContainer.o Search.o Search

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS = $(LIBSRC) Thread.h Makefile README

all: MapReduceFramework

test.o: uthreads.h
	$(CC) $(CFLAGS)  test.cpp -o test.o
ThreadToContainer.o: ThreadToContainer.cpp ThreadToContainer.h MapReduceClient.h
	$(CC) $(CFLAGS) ThreadToContainer.cpp -o ThreadToContainer.o

MapReduceFramework.o: MapReduceFramework.cpp MapReduceFramework.h MapReduceClient.h ThreadToContainer.h
	$(CC) $(CFLAGS) MapReduceFramework.cpp -o MapReduceFramework.o

Search.o: Search.cpp MapReduceFramework.h MapReduceClient.h
	$(CC) $(CFLAGS) Search.cpp -o Search.o

main: Search.o MapReduceFramework
	$(CC) -o Search Search.o MapReduceFramework.a

MapReduceFramework: MapReduceFramework.o ThreadToContainer.o
	ar rcs MapReduceFramework.a MapReduceFramework.o ThreadToContainer.o

clean:
	$(RM) $(TARGETS) $(TARNAME)


tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
