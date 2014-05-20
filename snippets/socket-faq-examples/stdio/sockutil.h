/* sockutil.h */

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <stdio.h>

int sock_read(int sock, void *buffer, int count);
int sock_write(int sock, const void *buffer, int count);
FILE *sock_to_file(int sock, const char *mode);

int inet_parseaddr(struct sockaddr_in *,
		   const char *addr, 
		   const char *proto,
		   const char *def_service);
unsigned short inet_service(const char *service, const char *proto);
unsigned long inet_hostaddr(const char *hostname);

/* eof */
