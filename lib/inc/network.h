#ifndef __NETWORK_H__
#define __NETWORK_H__

#define SERVER_TCP_DEFAULT_PORT 3000
#define SOCK_CMD_FILE_CLIENT_TO_SERVER   "0"
#define SOCK_CMD_LIST_SERVER_TO_CLIENT   "1"
#define SOCK_CMD_DELETE_CLIENT_TO_SERVER "2"
#define SOCK_CMD_PLAY_CLIENT_TO_SERVER   "3" 
#define SOCK_CMD_SENSOR_SERVER_TO_CLIENT "4"
#define SOCK_CMD_CAMERA_SERVER_TO_CLIENT "5"
#define SOCK_CMD_END                     "-1" 

int network_server_init(int port);
int network_accept_client(int sd);
void network_get_port(int argc, char **argv, int *port);

#endif
