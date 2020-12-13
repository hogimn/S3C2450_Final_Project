#ifndef __NETWORK_H__
#define __NETWORK_H__

#define SERVER_TCP_DEFAULT_PORT 3000
#define NETWORK_CMD_FILE_CLIENT_TO_SERVER   "0"
#define NETWORK_CMD_LIST_SERVER_TO_CLIENT   "1"
#define NETWORK_CMD_DELETE_CLIENT_TO_SERVER "2"
#define NETWORK_CMD_PLAY_CLIENT_TO_SERVER   "3" 
#define NETWORK_CMD_SENSOR_SERVER_TO_CLIENT "4"
#define NETWORK_CMD_CAMERA_SERVER_TO_CLIENT "5"
#define NETWORK_CMD_HUMI_SERVER_TO_CLIENT   "6"
#define NETWORK_CMD_TEMP_SERVER_TO_CLIENT   "7"
#define NETWORK_CMD_MODE_SERVER_TO_CLIENT   "8"
#define NETWORK_CMD_MODE_TOGGLE_CLIENT_TO_SERVER "9"
#define NETWORK_CMD_END                     "-1" 

#define NETWORK_BUFSIZE 1024

int network_server_init(int port);
int network_accept_client(int sd);
void network_get_port(int argc, char **argv, int *port);
int network_recv_poll(int sd, void *buf, int size);
int network_send(int sd, void *buf, int size);
int network_send_cmd_end(int sd);

#endif
