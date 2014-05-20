/* example program to speak SMTP to a mail server to verify addresses   */
/* written mostly as a test of using stdio + sockets, but seems to work */

#include "sockutil.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>


/* read a response from the server. If TIMEOUT (seconds) elapse, deliver */
/* SIGALRM to the process. Otherwise returns the response code, or 0 for EOF.*/
/* This version is chatty; all server responses are echoed to stdout.    */

int read_response(FILE *fp, int timeout)
{
    char buf[1024];
    char *ptr;

    for (;;)
    {
	alarm(timeout);
	ptr = fgets(buf, 1023, fp);
	alarm(0);

	if (!ptr)
	{
	    puts("<EOF>");
	    return 0;
	}

	ptr += strlen(ptr);

	if (*--ptr == '\n')
	    if (*--ptr == '\r')
		*ptr = '\0';

	printf("%s\n", buf);

	if (strlen(buf) < 3 || strspn(buf, "0123456789") != 3)
	{
	    puts("*** malformed response");
	}
	else if (buf[3] == ' ' || buf[3] == '\0')
	{
	    return atoi(buf);
	}
    }
}	    


/* format a command string and send it to the server. */
/* maximum command length 1021 bytes */
/* chatty version; emits the command to stdout as well */

int send_command(FILE *fp, const char *fmt, ...)
{
    char buf[1024];
    va_list va;
    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    va_end(va);

    strcat(buf,"\n");
    fputs(buf, stdout);
    strcpy(buf + strlen(buf) - 1, "\r\n");
    return fputs(buf, fp) != EOF;
}


/* Usage: smtp_vrfy server addr addr... */
/* 'server' can be just a name, or name:port */

int main(int argc, char *argv[])
{
    struct sockaddr_in addr;
    int sock;
    int rc;
    FILE *rfp;
    FILE *wfp;
    char buf[1024];
    int tmout = 5*60;   /* 5 minutes should be long enough */

    if (argc < 2)
    {
	fprintf(stderr, "Insufficient arguments\n");
	exit(2);
    }

    /* grok the server name */

    if (!inet_parseaddr(&addr, argv[1], "tcp", "smtp"))
    {
	fprintf(stderr, "Can't parse '%s' as host/service\n", argv[1]);
	exit(2);
    }

    /* lose program name and server name from argv */

    argc -= 2;
    argv += 2;

    /* make the socket */

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
	perror("socket");
	exit(1);
    }

    /* contact server */

    rc = connect(sock, &addr, sizeof(addr));
    if (rc < 0)
    {
	perror("connect");
	exit(1);
    }

    /* create read and write streams */

    rfp = sock_to_file(sock, "rt");
    wfp = sock_to_file(dup(sock), "wt");

    /* first response; expect 220 from server */

    if (read_response(rfp, tmout) != 220)
    {
	fprintf(stderr,"Unexpected response from server.\n");
	exit(1);
    }

    /* who are we? */

    gethostname(buf, MAXHOSTNAMELEN);

    /* be polite */

    send_command(wfp, "HELO %s", buf);
    read_response(rfp, tmout);

    /* iterate over arguments, emitting a VRFY command for each */
    /* we are echoing the server responses anyway */

    while (argc--)
    {
	send_command(wfp, "VRFY %s", *argv++);
	read_response(rfp, tmout);
    }

    /* all done */

    send_command(wfp, "QUIT");
    read_response(rfp, tmout);

    /* This one should get EOF */

    read_response(rfp, tmout);

    /* close the streams and exit */

    fclose(rfp);
    fclose(wfp);

    return 0;
}
