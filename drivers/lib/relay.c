#include "relay.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

static int fd_relay;

int relay_init(void)
{
	fd_relay = open(RELAY_DEV_PATH, O_RDWR,0);
	
	if (fd_relay<0) {
       // file open error
       return !RELAY_INIT_OK;
    }
	
	ioctl(fd_relay, IN1_HIGH, 0);
	ioctl(fd_relay, IN2_HIGH, 0);
	ioctl(fd_relay, IN3_HIGH, 0);
	ioctl(fd_relay, IN4_HIGH, 0);
	
	return RELAY_INIT_OK;
}

int relay_connect(int channel)
{
	switch(channel)
	{
		case 1:
			ioctl(fd_relay, IN1_LOW, 0);
			break;
		case 2:
			ioctl(fd_relay, IN2_LOW, 0);
			break;
		case 3:
			ioctl(fd_relay, IN3_LOW, 0);
			break;
		case 4:
			ioctl(fd_relay, IN4_LOW, 0);
			break;
		default:
			// Unknown command
			return !RELAY_CONNECT_OK;
	}
	
	return RELAY_CONNECT_OK;
}

int relay_disconnect(int channel)
{	
	switch(channel)
	{
		case 1:
			ioctl(fd_relay, IN1_HIGH, 0);
			break;
		case 2:
			ioctl(fd_relay, IN2_HIGH, 0);
			break;
		case 3:
			ioctl(fd_relay, IN3_HIGH, 0);
			break;
		case 4:
			ioctl(fd_relay, IN4_HIGH, 0);
			break;
		default:
			// Unknown command
			return !RELAY_DISCONNECT_OK;
	}

	return RELAY_DISCONNECT_OK;
}

void relay_deinit(void)
{	
	ioctl(fd_relay, IN1_HIGH, 0);
	ioctl(fd_relay, IN2_HIGH, 0);
	ioctl(fd_relay, IN3_HIGH, 0);
	ioctl(fd_relay, IN4_HIGH, 0);
	
	close(fd_relay);
}
