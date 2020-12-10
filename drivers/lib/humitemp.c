#include "humitemp.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int fd_humitemp;
static char buf[5];

int humitemp_init(void)
{
    fd_humitemp = open(HUMITEMP_DEV_PATH, O_RDWR);
    if (fd_humitemp < 0)
    {
        // file open error
        return !HUMITEMP_INIT_OK;
    }
	return HUMITEMP_INIT_OK;
}

int humitemp_read(int humitemp[2])
{
    int count;

    count = read(fd_humitemp, buf, sizeof(buf)); 

    if (count == 5 && 
        buf[0] != -1 &&
        buf[2] != -1)
    {
        /* humidity */
        humitemp[0] = buf[0];
        /* temparature */
        humitemp[1] = buf[2];

        return HUMITEMP_READ_OK;
    }

    return !HUMITEMP_READ_OK;
}

void humitemp_deinit(void)
{
    close(fd_humitemp);
}
