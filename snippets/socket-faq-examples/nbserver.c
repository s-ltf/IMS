/* 
   PLEASE READ THE FILE NB-APOLOGY!!!!  There are some things you should
   know about this source before you read it.  Thanks.

   Non blocking server demo 
   By Vic Metcalfe (vic@acm.org)
   For the unix-socket-faq
*/

#include "sockhelp.h"
#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>

struct connect_info {
	int fd;			/* Socket file descriptor */
	char *input_buf;	/* Input buffer for reads from fd */
	char *output_buf;	/* Output buffer for writes to fd */
	size_t input_buf_sz;	/* Current size of input buffer */
	size_t output_buf_sz;	/* Current size of output buffer */
}

int sock;            /* The socket file descriptor for our "listening"
                   	socket */
struct connect_info connectlist[5];  /* Array of connected socket data
			so we know who we are talking to, and the state
			of our communication.  (File descriptor,
			input and output buffers)
fd_set socks;        /* Socket file descriptors we want to wake
			up for, using select() */
int highsock;	     /* Highest #'d file descriptor, needed for select() */

/* Does read into sock_info structure.  This is like the sock_gets in
sockhelp.c, but it uses the buffers in the connect_info struct so that
the whole string up to the LF is returned all at once, even though it
may have actually come in from multiple reads.  If the full string can
not be returned into buf, then it returns -1 and errno is set to
EAGAIN, so you should be prepared to call this function more than
once to retrieve a single line of text. */
int nb_sock_gets(sock_info, buf, count)
struct connect_info *sock_info;
char *buf;
size_t count;
{
	char read_buffer[100];
	int bytes_read;
	char *buffer_target;

	bytes_read = read(sock_info->fd,read_buffer,read_buffer,100);
	if (bytes_read < 0)
		return -1; /* Pass error up to caller */
	else if (bytes_read > 0) {
		/* Update sock_info buffer with new data */
		remalloc(sock_info->input_buf, sock_info->input_buf_sz +
			bytes_read); /* Expand/create input buffer */
		buffer_target = sock_info->input_buf;
		buffer_target += sock_info->input_sz;
		memcpy(buffer_target,read_buffer,bytes_read);
		sock_info->input_sz += bytes_read;
		endofline = memchr(buffer_target,10,bytes_read);
		if (endofline != NULL) {
			/* We have read to end of line.  Return the
			   string, and adjust the input buffer. */
			endofline++; /* Move past LF character */
			memmove(sock_info->input_buf,endofline,???);
		}
	}
	return 0; /* Not an error, no data read, so must be closed. */
}

void setnonblocking(sock)
int sock;
{
	int opts;

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
}

void build_select_list() {
	int listnum;	     /* Current item in connectlist for for loops */

	/* First put together fd_set for select(), which will
	   consist of the sock veriable in case a new connection
	   is coming in, plus all the sockets we have already
	   accepted. */
	FD_ZERO(&socks);
	FD_SET(sock,&socks);
	for (listnum = 0; listnum < 5; listnum++) {
		if (connectlist[listnum] != 0) {
			FD_SET(connectlist[listnum],&socks);
			if (connectlist[listnum] > highsock)
				highsock = connectlist[listnum];
		}
	}
}

void handle_new_connection() {
	int listnum;	     /* Current item in connectlist for for loops */
	int connection; /* Socket file descriptor for incoming connections */

	/* We have a new connection coming in!  We'll
	try to find a spot for it in connectlist. */
	connection = accept(sock, NULL, NULL);
	if (connection < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	setnonblocking(connection);
	for (listnum = 0; (listnum < 5) && (connection != -1); listnum ++)
		if (connectlist[listnum] == 0) {
			printf("\nConnection accepted:   FD=%d; Slot=%d\n",
				connection,listnum);
			connectlist[listnum] = connection;
			connection = -1;
		}
	if (connection != -1) {
		/* No room left in the queue! */
		printf("\nNo room left for new client.\n");
		sock_puts(connection,"Sorry, this server is too busy.  Try again later!\r\n");
		close(connection);
	}
}

void deal_with_data() {
	int listnum;	     /* Current item in connectlist for for loops */
	char buffer[80];     /* Buffer for socket reads */
	char *cur_char;      /* Used in processing buffer */

	if (sock_gets(connectlist[listnum],buffer,80) < 0) {
		/* Connection closed, close this end
		   and free up entry in connectlist */
		printf("\nConnection lost: FD=%d;  Slot=%d\n",
			connectlist[listnum],listnum);
		close(connectlist[listnum]);
		connectlist[listnum] = 0;
	} else {
		/* We got some data, so upper case it
		   and send it back. */
		printf("\nReceived: %s; ",buffer);
		cur_char = buffer;
		while (cur_char[0] != 0) {
			cur_char[0] = toupper(cur_char[0]);
			cur_char++;
		}
		sock_puts(connectlist[listnum],buffer);
		sock_puts(connectlist[listnum],"\n");
		printf("responded: %s\n",buffer);
	}
}

void read_socks() {
	int listnum;	     /* Current item in connectlist for for loops */

	/* OK, now socks will be set with whatever socket(s)
	   are ready for reading.  Lets first check our
	   "listening" socket, and then check the sockets
	   in connectlist. */
	if (FD_ISSET(sock,&socks))
		handle_new_connection();
	/* Now check connectlist for available data */
	for (listnum = 0; listnum < 5; listnum++) {
		if (FD_ISSET(connectlist[listnum],&socks))
			deal_with_data();
	} /* for (all entries in queue) */
}

int main (argc, argv) 
int argc;
char *argv[];
{
	char *ascport;  /* ASCII version of the server port */
	int port;       /* The port number after conversion from ascport */
	struct sockaddr_in server_address; /* bind info structure */
	int reuse_addr = 1;  /* Used so we can re-bind to our port
				while a previous connection is still
				in TIME_WAIT state. */
	struct timeval timeout;  /* Timeout for select */
	int readsocks;	     /* Number of sockets ready for reading */

	/* Make sure we got a port number as a parameter */
	if (argc < 2) {
		printf("Usage: %s PORT\r\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Obtain a file descriptor for our "listening" socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	/* So that we can re-bind to it without TIME_WAIT problems */
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
		sizeof(reuse_addr));

	/* Set socket to non-blocking with our setnonblocking routine */
	setnonblocking(sock);

	/* Get the address information, and bind it to the socket */
	ascport = argv[1]; /* Read what the user gave us */
	port = atoport(ascport); /* Use function from sockhelp to
                                    convert to an int */
	memset((char *) &server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = port;
	if (bind(sock, (struct sockaddr *) &server_address,
	  sizeof(server_address)) < 0 ) {
		perror("bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	/* Set up queue for incoming connections. */
	listen(sock,5);

	/* Since we start with only one socket, the listening socket,
	   it is the highest socket so far. */
	highsock = sock;
	memset((char *) &connectlist, 0, sizeof(connectlist));

	while (1) { /* Main server loop - forever */
		build_select_list();
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		readsocks = select(highsock+1, &socks, (fd_set *) 0, 
		  (fd_set *) 0, &timeout);
		if (readsocks < 0) {
			perror("select");
			exit(EXIT_FAILURE);
		}
		if (readsocks == 0) {
			/* Nothing ready to read, just show that
			   we're alive */
			printf(".");
			fflush(stdout);
		} else
			read_socks();
	} /* while(1) */
} /* main */

