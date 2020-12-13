#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "network.h"

int network_server_init(int port)
{
    struct sockaddr_in server;
    int rc; int sd;

    /*
     * when client socket is closed during connection
     * SIGPIPE will delivered to main program.
     * SIGPIPE will terminate the program by default.
     * ignore that signal
     */
    signal(SIGPIPE, SIG_IGN);

    /* create a stream socket */	
    sd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if (sd == -1) 
    {
        return sd;
    } 
    /* bind an address to the socket */
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    rc = bind(sd, (struct sockaddr *)&server, sizeof(server));
    if (rc == -1)
    {
        return rc;
    }

    /* queue up to 5 connect requests */
    listen(sd, 5);

    /* successful return value */
    return sd;
}

int network_accept_client(int sd)
{
    int new_sd;

    struct sockaddr_in client;
    socklen_t client_addrlen;

    client_addrlen = sizeof(client);

    new_sd = accept(sd, 
            (struct sockaddr *)&client, 
            &client_addrlen);

    return new_sd;
}

/* set port number manually if argc > 1 */
void network_get_port(int argc, char **argv, int *port)
{
    switch (argc)
    {
        case 1:
            *port = SERVER_TCP_DEFAULT_PORT;
            break;
        case 2:
            *port = atoi(argv[1]);
            break;
        default:
            fprintf(stderr, "Usage: %s [port]\n", argv[0]);
            exit(1);
    }
}

int network_recv_poll(int sd, void *buf, int size)
{
    int bytes_read;

    /* polling recv */
    bytes_read = recv(sd, buf, size, 0);

    return bytes_read;
}

int network_send(int sd, void *buf, int size)
{
    int rc;
    
    rc = send(sd, buf, size, 0);

    return rc;
}

int network_send_cmd_end(int sd)
{
    int rc;
    char buf[NETWORK_BUFSIZE];

    sprintf(buf, "%d\n", (const char *)NETWORK_CMD_END);
    rc = network_send(sd, buf, strlen(buf));

    return rc; 
}
