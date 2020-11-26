#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include "temphumid.h"

int main(void)
{
    int count = 0;

    temphumid_init();

    while (1)
    {
        if (count == 5)
            break;

        int rc;
        int data[2];
        rc = temphumid_read(data);
        if (rc == TEMPHUMID_OK)
            printf("%d, %d\n", data[0], data[1]);
        else
            printf("error!\n");
        sleep(1);

        count++;
    }

    temphumid_deinit();
    return 0;                  
}
