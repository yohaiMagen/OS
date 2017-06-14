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

all: CacheFS

CacheAlg.o: CacheAlg.cpp CacheAlg.h
	$(CC) $(CFLAGS)  CacheAlg.cpp -o CacheAlg.o

CacheFS.o: CacheFS.cpp CacheFS.h
	$(CC) $(CFLAGS)  CacheFS.cpp -o CacheFS.o

Lfu.o: Lfu.cpp Lfu.h CacheAlg.h
	$(CC) $(CFLAGS) Lfu.cpp -o Lfu.o

Lru.o: Lru.cpp Lru.h CacheAlg.h
	$(CC) $(CFLAGS) Lru.cpp -o Lru.o

Fbr.o: Fbr.cpp Fbr.h CacheAlg.h
	$(CC) $(CFLAGS) Fbr.cpp -o Fbr.o

CacheFS: CacheFS.o CacheAlg.o Lfu.o Lru.o Fbr.o
	ar rcs CacheFS.a CacheFS.o CacheAlg.o Lfu.o Lru.o Fbr.o


clean:
	$(RM) $(TARGETS) $(TARNAME)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
