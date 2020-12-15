#include "moisture.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

static int fd_moisture;

int moisture_init(void)
{
	fd_moisture = open(MOISTURE_DEV_PATH, O_RDWR, 0);
    if (fd_moisture < 0)
    {
        // file open error
        return !MOISTURE_INIT_OK;
    }
	
	return MOISTURE_INIT_OK;
}

int moisture_is_full(void)
{
	int ret = read(fd_moisture,NULL,0);
	
	return !ret;
}

void moisture_deinit(void)
{
	close(fd_moisture);
}
