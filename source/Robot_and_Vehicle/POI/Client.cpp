/*
 This program acts as a client in the Client-Server arcitecture
 of network communication.
 */

#include "Client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SEND_BUFFER 1024
#define RECEIVE_BUFFER 2048

int socketHandle;
struct hostent *hPtr;
struct sockaddr_in socketInfo;
int flag;   //represents socket flag
socklen_t res_size;
//timers used for sending & receiving data
struct timeval send_timer;
struct timeval receive_timer;
fd_set readfds; //read fd set
fd_set writefds; //write fd set

//NONBLOCK or BLOCK mode
#define BLOCK

/*
 Class Constructor
 Initializes a connection with a host process
 */
Client::Client(const char* host, int port)
{
    printf("Creating connection...\n");
    #ifdef NONBLOCK
	//set timer values
    receive_timer.tv_sec = 0;
    receive_timer.tv_usec = 1000;

    send_timer.tv_sec = 0;
    send_timer.tv_usec = 5000;
    #endif

    bzero(&socketInfo, sizeof(sockaddr_in));    //clear memory structure

    //get host information
    hPtr = gethostbyname(host);
    if (hPtr == NULL)
    {
        perror("Host lookup failed");
        exit(EXIT_FAILURE);
    }
	//create a socket
	socketHandle = socket(AF_INET, SOCK_STREAM, 0);
	//printf("Socket: %d\n", socketHandle);
	if (socketHandle < 0)
	{
		perror("Error creating socket");
		close(socketHandle);
		exit(EXIT_FAILURE);
	}

#ifdef NONBLOCK	//changing flag
    printf("Running non blocking mode...\n");
    //retrive socket flag
    flag = fcntl(socketHandle, F_GETFL);
    if (flag < 0)
    {
        printf("Unable to retrieve descriptor status flag");
        exit (EXIT_FAILURE);
    }
    //set non-blocking flag
    if (fcntl(socketHandle, F_SETFL, flag | O_NONBLOCK) < 0)
    {
        printf("Setting non-blocking flag failed");
        exit(EXIT_FAILURE);
    }
#endif

    //load host information into socket structure (sockaddr_in)
    memcpy((char *)&socketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
    socketInfo.sin_family = AF_INET;
    socketInfo.sin_port = htons((ushort)port); //convert to network byte order
    //connect
    int rc = connect(socketHandle, (struct sockaddr*)&socketInfo, sizeof(sockaddr_in));
	//printf("Connect Value: %d\n", rc);

#ifdef NONBLOCK
    if(rc < 0 && errno == EINPROGRESS)
    {
        res_size = sizeof(rc);
        //obtain socket level error option
        getsockopt(socketHandle, SOL_SOCKET, SO_ERROR, &rc, &res_size);
        //if SO_ERROR is 0, connection completed successfully
        if(rc == EISCONN)
            rc = 0;
        errno = rc;
        //ABOVE SHOULD BE IF TIMEOUT = 0
    }
    else
    {
        printf("Unable to connect. %s. Error number: %d\n", strerror(errno), errno);
        close(socketHandle);
        exit(EXIT_FAILURE);
    }
#endif

#ifdef BLOCK
    if(rc < 0)
    {
        printf("Unable to connect. %s. Error number: %d\n", strerror(errno), errno);
        close(socketHandle);
        exit(EXIT_FAILURE);
    }
#endif
    printf("Connection successfull\n");
}

/*
 This function sends data synchronously to the server 
 */
void Client::Send(const char* data)
{
    int rc;
    int size;
    char buf[SEND_BUFFER];
    strcpy(buf, data);
#ifdef NONBLOCK
    //clear the set
    FD_ZERO(&writefds);
    //add socket descriptor to set
    FD_SET(socketHandle, &writefds);
    //send data when descriptor is ready for I/O operation
    rc = select(socketHandle+1, NULL, &writefds, NULL, &send_timer);
    //check if descriptor is ready
    if (rc == -1)
    {
        printf("select error\n");
    }
    else if(FD_ISSET(socketHandle, &writefds)) //rc = 1
    {
        size = send(socketHandle, buf, strlen(buf)+1, 0);
        if (size < 0)
        {
            printf("Error sending data");
        }
        else if(size == 0)
        {
            printf("Socket closed");
        }
        else
        {
            printf("Sent %d bytes\n", (int)strlen(buf)+1);
        }
    }
    else
    {
        printf("Timeout\n");
    }
#endif
#ifdef BLOCK
    size = send(socketHandle, buf, strlen(buf)+1, 0);
    if (size < 0)
    {
        printf("Error sending data");
    }
    else if(size == 0)
    {
        printf("Socket closed");
    }
    else
    {
        printf("Sent %d bytes\n", (int)strlen(buf)+1);
    }
#endif
}
/*
 This function reads data from the socket when it's available
 */
string Client::Receive()
{
    int rc;
    int size;
    string data;
    char buf[RECEIVE_BUFFER];
    
#ifdef NONBLOCK
    //clear fd set
    FD_ZERO(&readfds);
    //add socket descriptor to set
    FD_SET(socketHandle, &readfds);
    //read data when something is available

    rc = select(socketHandle+1, &readfds, NULL, NULL, &receive_timer);
    if (rc == -1)
    {
        printf("Select error\n");
        return data = "";
    }
    else if(FD_ISSET(socketHandle, &readfds))   //rc = 1
    {
        size = recv(socketHandle, buf, strlen(buf)+1, MSG_PEEK);
        if (size < 0)
        {
            printf("recv error\n");
            return data = "";
        }
        else if(size == 0)
        {
            printf("No data...\n");
            return data = "";
        }
        else
        {
            printf("Received %d bytes...\n", (int)strlen(buf)+1);
            data = string(buf, size);
            return data;
        }
    }
    else
    {
        printf("No data: timeout\n");
        return data = "";
    }
#endif

#ifdef BLOCK
    //  since we are receiving only one type of data...maybe keep a fixed size?
    size = recv(socketHandle, buf, 10, MSG_WAITALL);
    if (size < 0)
    {
        printf("recv error\n");
        return data = "";
    }
    else if(size == 0)
    {
        printf("No data...\n");
     	return data = "";
    }
    else
    {
        printf("Received %d bytes...\n", (int)strlen(buf)+1);
	data = string(buf, size);
	cout << "Received: " << data << endl;
        return data;
    }
#endif
    return data = "";
}
