#ifndef __MAGNETIC_H__
#define __MAGNETIC_H__

#define MAGNETIC_DEV_PATH  "/dev/mds2450_szh_ssbh_040"

#define MAGNETIC_NULL     0
#define MAGNETIC_DETECTED 1
#define MAGNETIC_INIT_OK  1

int magnetic_init(void);
int magnetic_is_detected(void);
void magnetic_deinit(void);

#endif
