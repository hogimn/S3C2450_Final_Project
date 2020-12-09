#ifndef __PHOTO_H__
#define __PHOTO_H__

int photo_init(void);
int photo_get_intensity(void);
void photo_deinit(void);

#define PHOTO_INIT_OK              1
#define PHOTO_INIT_FILE_OPEN_FAIL -1
#define PHOTO_INIT_I2C_FAIL       -2

#endif