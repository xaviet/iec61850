# flags
LDFLAGS = -lpthread
CFLAGS = -Wall -v -H -g
CC = gcc $(CFLAGS) $(LDFLAGS)
# source
OBJECT = main.o rawsocket.o sometools.o goosepub.o svpub.o 61850.o
APPNAME = iec61850pub
# command
all:	$(APPNAME)
$(APPNAME):	$(OBJECT)
	$(CC) $(OBJECT) -o $(APPNAME)
clean:
	rm -f $(OBJECT) $(APPNAME)

