#ifndef __TEMPHUMID_H__
#define __TEMPHUMID_H__

#define TEMPHUMID_DEV_PATH  "/dev/mds2450_dht11"
#define TEMPHUMID_OK 1

void temphumid_init(void);
int temphumid_read(int ret[2]);
void temphumid_deinit(void);

#endif
