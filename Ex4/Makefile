CC=g++
RANLIB=ranlib

LIBSRC= uthreads.cpp Thread.cpp


INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11  
LOADLIBES = -L./ 

TARGETS = *.o CacheFS.a test1 test

TAR=tar
TARFLAGS=-cvf
TARNAME=ex3.tar
TARSRCS = CacheFS.cpp CacheAlg.cpp CacheAlg.h Lfu.cpp Lfu.h Lru.cpp Lru.h Fbr.cpp Fbr.h README Makefile

all: CacheFS test

CacheFS.o: CacheFS.cpp CacheFS.h
	$(CC) $(CFLAGS)  CacheFS.cpp -o CacheFS.o

Lfu.o: Lfu.cpp Lfu.h CacheAlg.h
	$(CC) $(CFLAGS) Lfu.cpp -o Lfu.o

Lru.o: Lru.cpp Lru.h CacheAlg.h
	$(CC) $(CFLAGS) Lru.cpp -o Lru.o

Fbr.o: Fbr.cpp Fbr.h CacheAlg.h
	$(CC) $(CFLAGS) Fbr.cpp -o Fbr.o

TEST.o: TEST.cpp CacheFS.h
	$(CC) $(CFLAGS) TEST.cpp -o TEST.o

test: TEST.o CacheFS
	$(CC)  -o test TEST.o CacheFS.a

CacheFS: CacheFS.o CacheFS.o Lfu.o  
	ar rcs CacheFS.a CacheFS.o CacheFS.o Lfu.o

test1: Lfu.o TEST.o CacheFS
	$(CC) -o test1 TEST.o CacheFS.a

clean:
	$(RM) $(TARGETS) $(TARNAME)


tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
