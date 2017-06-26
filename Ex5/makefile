CC=g++
RANLIB=ranlib

LIBSRC= uthreads.cpp Thread.cpp


INCS=-I.
CFLAGS = -Wall -Wextra -g -c -std=c++11
LOADLIBES = -L./

TARGETS = *.o whatsappServer whatsappClient

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS =  README makefile Server.cpp utilities.h Client.cpp utilities.cpp whatsapp_exception.h

all: server client

Server.o: Server.cpp utilities.h whatsapp_exception.h
	$(CC) $(CFLAGS)  Server.cpp -o Server.o

Client.o: Client.cpp utilities.h whatsapp_exception.h
	$(CC) $(CFLAGS) Client.cpp -o Client.o

utilities.o: utilities.cpp utilities.h whatsapp_exception.h
	$(CC) $(CFLAGS) utilities.cpp -o utilities.o

server: Server.o utilities.o
	$(CC) -o whatsappServer Server.o utilities.o

client: Client.o utilities.o
	$(CC) -o whatsappClient Client.o utilities.o



clean:
	$(RM) $(TARGETS) $(TARNAME)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
