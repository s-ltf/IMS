/*
 *  This file is provided for use with the unix-socket-faq.  It is public
 *  domain, and may be copied freely.  There is no copyright on it.  The
 *  original work was by Vic Metcalfe (vic@brutus.tlug.org), and any
 *  modifications made to that work were made with the understanding that
 *  the finished work would be in the public domain.
 *
 *  If you have found a bug, please pass it on to me at the above address
 *  acknowledging that there will be no copyright on your work.
 *
 *  The most recent version of this file, and the unix-socket-faq can be
 *  found at http://www.interlog.com/~vic/sock-faq/.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "sockhelp.h"

int main(argc, argv)
int argc;
char *argv[];
{
  int sock;  /* Sock we will talk on */
  struct sockaddr_in client, server; /* Address information for client 
                                        and server */
  int recvd; /* Number of bytes recieved */
  int thisinc; /* How many should we increment this time? */
  int count = 0; /* Our current count */
  int structlength; /* Length of sockaddr structure */
  int port; /* The port we will talk on. */

  if (argc < 2) {
    fprintf(stderr,"Usage: udpserver service\n");
    fprintf(stderr,"where service is a udp port name from /etc/services,\n");
    fprintf(stderr,"or a port number.\n");
    exit(EXIT_FAILURE);
  }

  port = atoport(argv[1],"udp"); /* atoport is from sockhelp.c */
  if (port < 0) {
    fprintf(stderr,"Unable to find port %s.\n",argv[1]);
    exit(EXIT_FAILURE);
  }

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Really there is no need for both a client and server structure
     here.  Both are initialized to the same values, and the server
     structure is only needed for the bind, so one would have done
     the trick.  Two were used in the hope of making the source code
     clearer to read.  You bind to the server port, accepting
     messages from anywhere.  You recvfrom a client, and get the
     client information in onother structure, so that you know who
     to repond to. */

  memset((char *) &client, 0, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  client.sin_port = port;

  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = port;

  structlength = sizeof(server);
  if (bind(sock, (struct sockaddr *) &server, structlength) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  while (1) {
    /* Get an increment request */
    structlength = sizeof(client);
    recvd = recvfrom(sock, &thisinc, sizeof(thisinc), 0, 
      (struct sockaddr *) &client, &structlength);
    if (recvd < 0) {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }
    if (recvd == sizeof(thisinc)) {
      thisinc = ntohl(thisinc);
      count += thisinc;
      printf("Adding %d.  Count now at %d.\n",thisinc,count);
      count = htonl(count);
      /* Send back the current total */
      if (sendto(sock, &count, sizeof(count), 0, (struct sockaddr *) &client, 
          structlength) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }
      count = ntohl(count);
    }
  }
}
