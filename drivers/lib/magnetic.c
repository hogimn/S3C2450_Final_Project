#include "magnetic.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

static int fd_magnetic;

int magnetic_init(void)
{
    fd_magnetic = open(MAGNETIC_DEV_PATH, O_RDWR);
    if (fd_magnetic < 0)
    {
        // file open error
        return !MAGNETIC_INIT_OK;
    }
	return MAGNETIC_INIT_OK;
}

// detected -> return 1
int magnetic_is_detected(void)
{
	int ret;
	
	ret = read(fd_magnetic,NULL,0);

	return !ret;
}

void magnetic_deinit(void)
{
	close(fd_magnetic);
}
