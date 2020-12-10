#ifndef __HUMITEMP_H__
#define __HUMITEMP_H__

#define HUMITEMP_DEV_PATH  "/dev/mds2450_dht11"

#define HUMITEMP_INIT_OK	1
#define HUMITEMP_READ_OK	1

int humitemp_init(void);
int humitemp_read(int humitemp[2]);
void humitemp_deinit(void);

#endif
