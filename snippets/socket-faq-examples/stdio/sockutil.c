/* sockutil.c - utility functions for sockets */

#include "sockutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>


/*
** sock_read
**
** Attempt to read COUNT bytes from socket SOCK into BUFFER.
** Returns number of bytes read, or -1 if an error occurs.
** Will read less than the specified count *only* if the peer sends
** EOF
*/

int sock_read(int sock, void *buffer, int count)
{
    register char *ptr = (char *) buffer;
    register int bytesleft = count;

    do
    {
	register int rc;

	do
	    rc = read(sock, ptr, bytesleft);
	while (rc < 0 && errno == EINTR);

	if (rc == 0)
	    return count - bytesleft;
	else if (rc < 0)
	    return -1;

	bytesleft -= rc;
	ptr += rc;
    }
    while (bytesleft);

    return count;
}

/*
** sock_write
**
** Attempt to write COUNT bytes to socket SOCK from BUFFER.
** Returns number of bytes written, or -1 if an error occurs.
** Return value will always be either -1 or COUNT
*/

int sock_write(int sock, const void *buffer, int count)
{
    register const char *ptr = (const char *) buffer;
    register int bytesleft = count;

    do
    {
	register int rc;

	do
	    rc = write(sock, ptr, bytesleft);
	while (rc < 0 && errno == EINTR);

	if (rc < 0)
	    return -1;

	bytesleft -= rc;
	ptr += rc;
    }
    while (bytesleft);

    return count;
}

/*
** sock_to_file
**
** Create a stdio stream from a socket
**
** The stream is unidirectional; dup() the socket and create 2 streams if
** you want bidirectionality.
**
** Modes:
**   r   default read mode
**   w   default write mode
**   rt  text read mode (line-buffered)
**   wt  text write mode (line-buffered)
**   rb  binary read mode (large full buffers)
**   wb  binary write mode (large full buffers)
**
** No translation of data is implied by the references to 'text' & 'binary'
*/

/* buffers less than 4k are inefficient on many implementations */
/* 28k writes to a 56k socket buffer is a magic combination on some */
/* high-speed networks, and should give near-optimal throughput on */
/* almost any system */

#define SOCK_SMALLBUF (4*1024)
#define SOCK_LARGEBUF (28*1024)

FILE *sock_to_file(int sock, const char *mode)
{
    int bufsz = SOCK_SMALLBUF;
    int bufmode = _IOFBF;

    FILE *fp = fdopen(sock, (*mode == 'w') ? "w" : "r");

    switch (mode[1])
    {
	case 't':
	    bufmode = _IOLBF;
	    break;

	case 'b':
	    bufsz = SOCK_LARGEBUF * 2;
	    setsockopt(sock, SOL_SOCKET,
		       (*mode == 'w') ? SO_SNDBUF : SO_RCVBUF,
		       &bufsz, sizeof(bufsz));
	    bufsz = SOCK_LARGEBUF;
	    break;
    }

    setvbuf(fp, NULL, bufmode, bufsz);

    return fp;
}


/* return the address of a hostname (or a dotted-quad address) */
/* return value is in NETWORK order */

unsigned long inet_hostaddr(const char *hostname)
{
    unsigned long addr = inet_addr(hostname);

    if (addr == INADDR_NONE)
    {
	struct hostent *host = gethostbyname(hostname);

	if (host)
	    memcpy(&addr, host->h_addr_list[0], sizeof(addr));
    }

    return addr;
}


/* return the port number (in NETWORK order) of a service */
/* service may be numeric, or a service name & protocol */

unsigned short inet_service(const char *service, const char *proto)
{
    unsigned short port = 0;
    char *endp;
    long temp;

    if (!service)
	return 0;

    temp = strtol((char *) service, &endp, 0);

    if (*endp == '\0' && temp > 0 && temp < 65536)
    {
	port = htons((unsigned short) temp);
    }
    else
    {
	struct servent *serv = getservbyname(service, proto);

	if (serv)
	    port = serv->s_port;
    }

    return port;
}


/* accepts strings of the form host:service or host:port */
/* if DEF_SERVICE is non-null, it is used if not specified in ADDR */
/* PROTO is used to resolve service names if required */
/* returns true if address could be parsed */

int inet_parseaddr(struct sockaddr_in *sa,
		   const char *addr,
		   const char *proto,
		   const char *def_service)
{
    char *host = strdup(addr);
    char *serv = strchr(host, ':');
    int ok = 0;

    if (host)
    {
	sa->sin_family = AF_INET;

	if (serv)
	    *serv++ = '\0';
	else
	    serv = (char *) def_service;

	sa->sin_port = inet_service(serv, proto);
	sa->sin_addr.s_addr = inet_hostaddr(host);

	if (sa->sin_addr.s_addr != INADDR_NONE && sa->sin_port > 0)
	    ok = 1;
    }
    
    free(host);

    return ok;
}
