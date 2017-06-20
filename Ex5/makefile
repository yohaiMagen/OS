CC=g++
RANLIB=ranlib

LIBSRC= uthreads.cpp Thread.cpp


INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11
LOADLIBES = -L./

TARGETS = *.o CacheFS.a

TAR=tar
TARFLAGS=-cvf
TARNAME=ex4.tar
TARSRCS = CacheFS.cpp CacheAlg.cpp CacheAlg.h Lfu.cpp Lfu.h Lru.cpp Lru.h Fbr.cpp Fbr.h README makefile Answers.pdf

all: server client

Server.o: Server.cpp utilities.h
	$(CC) $(CFLAGS)  Server.cpp -o Server.o

Client.o: Client.cpp utilities.h
	$(CC) $(CFLAGS) Client.cpp -o Client.o

utilities.o: utilities.cpp utilities.h
	$(CC) $(CFLAGS) utilities.cpp -o utilities.o

server: Server.o utilities.o
	$(CC) -o server Server.o utilities.o

client: Client.o utilities.o
	$(CC) -o client Client.o utilities.o



clean:
	$(RM) $(TARGETS) $(TARNAME)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
