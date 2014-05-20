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

#include "sockhelp.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

int listensock = -1; /* So that we can close sockets on ctrl-c */
int connectsock = -1;

/* This waits for all children, so that they don't become zombies. */
void sig_chld(signal_type)
int signal_type;
{
  int pid;
  int status;

  while ( (pid = wait3(&status, WNOHANG, NULL)) > 0);
}

int main(argc, argv)
int argc;
char *argv[];
{
  int sock;
  int connected = 1;
  char buffer[1024];
  char *current_character;
  int port = -1;
  struct sigaction act, oldact;

  if (argc != 2) {
    fprintf(stderr,"Usage:  tcpserver port\n");
    fprintf(stderr,"Where port is the port number or service name to\n");
    fprintf(stderr,"listen to.\n");
    exit(EXIT_FAILURE);
  }
  ignore_pipe();
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = sig_chld;
  sigaction(SIGCHLD, &act, &oldact);

  port = atoport(argv[1], "tcp");
  if (port == -1) {
    fprintf(stderr,"Unable to find service: %s\n",argv[1]);
    exit(EXIT_FAILURE);
  }

  sock = get_connection(SOCK_STREAM, port, &listensock);

  connectsock = sock;
  sock_puts(sock,"Welcome to the upper case server.\n");
  while (connected) {
    /* Read input */
    if ( sock_gets(sock, buffer, 1024) < 0) {
      connected = 0;
    }
    else {
      /* Upper case input */
      current_character = buffer;
      while (current_character[0] != 0) {
        current_character[0] = toupper(current_character[0]);
        current_character++;
      }
      /* Echo back upper cased input, plus linefeed */
      strcat(buffer, "\n");
      if (sock_puts(sock, buffer) < 0) {
        connected = 0;
      }
    }
  }
  close(sock);
  return 0;
}
