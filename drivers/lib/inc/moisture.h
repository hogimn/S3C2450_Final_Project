#ifndef __MOISTURE_H__
#define __MOISTURE_H__

#define MOISTURE_DEV_PATH  "/dev/mds2450_fc_28"

#define MOISTURE_INIT_OK	1

#define MOISTURE_LACK    0
#define MOISTURE_FULL    1

int moisture_init(void);
int moisture_is_full(void);
void moisture_deinit(void);

#endif
