#include "temphumid.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int fd_temphumid;
static char buf[5];

void temphumid_init(void)
{
    fd_temphumid = open(TEMPHUMID_DEV_PATH, O_RDWR);
    if (fd_temphumid < 0)
    {
        perror("temphumid_init() failed\n");
        exit(0);
    }
}

int temphumid_read(int ret[2])
{
    int count;

    count = read(fd_temphumid, buf, sizeof(buf)); 

    if (count == 5 && 
        buf[0] != -1 &&
        buf[2] != -1)
    {
        ret[0] = buf[0];
        ret[1] = buf[2];
        return TEMPHUMID_OK;
    }

    return !TEMPHUMID_OK;
}

void temphumid_deinit(void)
{
    close(fd_temphumid);
}
