# Uncomment the following line for Solaris
# C_LINK = -lsocket -lnsl

# Uncomment this for SCO.  (Note, this has only been reported to work with
# Revision 3.2.4 with the "SCO TCP/IP Development System" package installed.
# Please let me know if you have any other SCO success stories.
# C_LINK = -lsocket

# Comment the following line if you are not using the gnu c compiler
# C_ARGS = -Wall

# You might have to change this if your c compiler is not cc
CC = cc

# You shouldn't need to make any more changes below this line.

all:	tcpclient tcpserver udpclient udpserver nbserver

clean:
	rm -f *.o

distclean:	clean
	rm -f tcpclient tcpserver udpclient udpserver nbserver

tcpclient:	tcpclient.o sockhelp.o
	$(CC) -o tcpclient tcpclient.o sockhelp.o $(C_LINK)

tcpserver:	tcpserver.o sockhelp.o
	$(CC) -o tcpserver tcpserver.o sockhelp.o $(C_LINK)

tcpclient.o:	tcpclient.c
	$(CC) $(C_ARGS) $(C_LINK) -c tcpclient.c

tcpserver.o:	tcpserver.c
	$(CC) $(C_ARGS) -c tcpserver.c

sockhelp.o:	sockhelp.c
	$(CC) $(C_ARGS) $(C_LINK) -c sockhelp.c

udpserver:	udpserver.o sockhelp.o
	$(CC) -o udpserver udpserver.o sockhelp.o $(C_LINK)

udpclient:	udpclient.o sockhelp.o
	$(CC) -o udpclient udpclient.o sockhelp.o $(C_LINK)

nbserver:	nbserver.o sockhelp.o
	$(CC) -o nbserver nbserver.o sockhelp.o $(C_LINK)

udpserver.o:	udpserver.c
	$(CC) $(C_ARGS)  -c udpserver.c

nbserver.o:	nbserver.c
	$(CC) $(C_ARGS)  -c nbserver.c

udpclient.o:	udpclient.c
	$(CC) $(C_ARGS) -c udpclient.c

reuseaddr:	reuseaddr.o sockhelp.o
	$(CC) -o reuseaddr reuseaddr.o sockhelp.o $(C_LINK)

reuseaddr.o:	reuseaddr.c
	$(CC) $(C_ARGS) -c reuseaddr.c
