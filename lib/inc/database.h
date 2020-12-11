#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "sqlite3.h"
#include "list.h"

sqlite3 *db;
List *list_humitemp;

void database_init(const char *filename);
void database_deinit(void);
void database_humitemp_insert(int humi, int temp);
void database_photo_insert(int photo);
void database_water_inset(int water);
void database_magnetic_insert(int magnetic);
void database_moisture_insert(int moisture);

void database_retrieve_list_init(void);
void database_humitemp_retrieve_all(void);

#endif
