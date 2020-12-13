#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include "humitemp.h"

int main(void)
{
    humitemp_init();

    while (1)
    {
        int rc;
        int humitemp[2];

        rc = humitemp_read(humitemp);
        if (rc != HUMITEMP_READ_OK)
        {
            printf("humitemp_read() failed\n");
            sleep(2);
            continue;
        }

        printf("humi: %d, temp: %d\n", humitemp[0], humitemp[1]);

        sleep(2);
    }

    humitemp_deinit();
    return 0;                  
}
