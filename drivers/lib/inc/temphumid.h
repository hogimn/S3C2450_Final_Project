#ifndef __TEMPHUMID_H__
#define __TEMPHUMID_H__

#define TEMPHUMID_DEV_PATH  "/dev/mds2450_dht11"

#define TEMPHUMID_INIT_OK	1
#define TEMPHUMID_READ_OK	1

int temphumid_init(void);
int temphumid_read(int ret[2]);
void temphumid_deinit(void);

#endif
