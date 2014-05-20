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
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "sockhelp.h"

int main(argc, argv)
int argc;
char *argv[];
{
  int sock; /* Our socket that we will talk through */
  struct sockaddr_in client, server; /* Client and server address info */
  int recvd; /* Number of bytes recieved */
  int incammount; /* Amount we want to incement the server's number */
  int total; /* New total, returned from server */
  int structlength; /* Length of sockaddr structure */
  int port; /* Port we will talk on */
  struct in_addr *addr; /* Internet address of server */
  char *err; /* Used to test for valid integer value */

  if (argc < 4) {
    fprintf(stderr,"Usage: udpclient address service increment \n");
    fprintf(stderr,"where address is an ip address, or host name, and\n");
    fprintf(stderr,"service is a service name from /etc/services, or a\n");
    fprintf(stderr,"port number.  Increment is the number to add to the\n");
    fprintf(stderr,"server's count.\n");
    exit(EXIT_FAILURE);
  }

  port = atoport(argv[2], "udp"); /* atoport is from sockhelp.c */
  if (port == -1) {
    fprintf(stderr,"Can't use port: %s\n",argv[3]);
    exit(EXIT_FAILURE);
  }

  addr = atoaddr(argv[1]); /* atoaddr is from sockhelp.c */
  if (addr == NULL) {
    fprintf(stderr,"Invalid network address: %s\n",argv[2]);
    exit(EXIT_FAILURE);
  }

  incammount = strtol(argv[3],&err,0);
  if (err[0] != 0) {
    fprintf(stderr,"%s is not a valid integer number.\n",argv[3]);
    exit(EXIT_FAILURE);
  }
  incammount = htonl(incammount);

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* We need two sockaddr_in structures here.  One is bound to the socket
     we want to talk on, and the other is used to indicate who we want to
     talk to. */

  /* Set up the server info */
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = addr->s_addr;
  server.sin_port = port;

  /* Set up another structure for our local socket */
  memset((char *) &client, 0, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  client.sin_port = htons(0);
  if (bind(sock, (struct sockaddr *) &client, sizeof(client)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* Send request for increment to server.  Note that all amounts are
     transmitted in network byte order, so that the client and server
     can run on different architectures. */

  if (sendto(sock, &incammount, sizeof(incammount), 0, 
    (struct sockaddr *) &server, sizeof(server)) < 0 ) { 
    perror("sendto");
    exit(EXIT_FAILURE);
  }

  /* Then wait for new total amount. */
  structlength = sizeof(client);
  recvd = recvfrom(sock, &total, sizeof(total), 0,
    (struct sockaddr *) &client, &structlength);
  if (recvd < 0) {
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }
  if (recvd != sizeof(total)) {
    fprintf(stderr,"Got back wrong number of bytes!\n");
    exit(EXIT_FAILURE);
  }
  total = ntohl(total);
  printf("Current count: %i\n",total);
  return 0;
}
