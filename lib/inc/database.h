#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "sqlite3.h"

#define DATABASE_HUMI       1
#define DATABASE_TEMP       2
#define DATABASE_PHOTO      3
#define DATABASE_MAGNET     4
#define DATABASE_MOISTURE   5

sqlite3 *db;

void database_init(const char *filename);
void database_deinit(void);
void database_insert(int table, int sensor);
void database_data_socket_transfer(int sd, int sensor);

#endif
