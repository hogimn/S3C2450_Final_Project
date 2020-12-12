#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "sqlite3.h"

#define DATABASE_SOCKET_HUMI 1
#define DATABASE_SOCKET_TEMP 2

sqlite3 *db;

void database_init(const char *filename);
void database_deinit(void);
void database_humitemp_insert(int humi, int temp);
void database_photo_insert(int photo);
void database_water_insert(int water);
void database_magnetic_insert(int magnetic);
void database_moisture_insert(int moisture);
void database_data_socket_transfer(int sd, int sensor);

#endif
