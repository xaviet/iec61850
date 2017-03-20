# flags
LDFLAGS = -lpthread
CFLAGS = -Wall -v -g -H
CC = gcc $(CFLAGS) $(LDFLAGS)
# source
OBJECT = main.o rawsocket.o sometools.o goosepub.o svpub.o
APPNAME = iec61850pub
# command
all:	$(APPNAME)
$(APPNAME):	$(OBJECT)
	$(CC) $(OBJECT) -o $(APPNAME)
clean:
	rm -f $(OBJECT) $(APPNAME)

