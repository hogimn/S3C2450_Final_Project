#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "error.h"

#define DATABASE_SQL_SIZE 128

static int database_humitemp_retrieve_callback(void *unused, int count, char **data, char **columns);

void database_init(const char *filename)
{
    int rc;
    char *err_msg;

    rc = sqlite3_open(filename, &db);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        ERR_HANDLE;
    } 
    /* create tables if not exists */
    char *sql = 
        "CREATE TABLE IF NOT EXISTS\
            humitemp(id INTEGER PRIMARY KEY, humi INT, temp INT);\
         CREATE TABLE IF NOT EXISTS\
            photo(id INTEGER PRIMARY KEY, photo INT);\
         CREATE TABLE IF NOT EXISTS\
            water(id INTEGER PRIMARY KEY, water INT);\
         CREATE TABLE IF NOT EXISTS\
            magnetic(id INTEGER PRIMARY KEY, magnetic INT);\
         CREATE TABLE IF NOT EXISTS\
            moisutre(id INTEGER PRIMARY KEY, moisture INT);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("%s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }

    database_retrieve_list_init();
}

void database_deinit(void)
{
    int rc;

    rc = sqlite3_close(db);
    if (rc != SQLITE_OK)
    {
        ERR_HANDLE;
    }
}

void database_humitemp_insert(int humi, int temp)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];
    sprintf(sql, "INSERT INTO humitemp (humi, temp) VALUES (%d, %d);",
        temp, humi);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

void database_photo_insert(int photo)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];
    sprintf(sql, "INSERT INTO photo (photo) VALUES (%d);",
        photo);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

void database_water_inset(int water)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];
    sprintf(sql, "INSERT INTO water (water) VALUES (%d);",
        water);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

void database_magnetic_insert(int magnetic)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];
    sprintf(sql, "INSERT INTO magnetic (magnetic) VALUES (%d);",
        magnetic);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

void database_moisture_insert(int moisture)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];
    sprintf(sql, "INSERT INTO moisture (moisture) VALUES (%d);",
        moisture);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

void database_retrieve_list_init(void)
{
    list_humitemp = (List *) malloc(sizeof(List));
    list_init(list_humitemp, free);
}

void database_retrieve_list_deinit(void)
{
    list_destroy(list_humitemp);
}

void database_humitemp_retrieve_all(void)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];
    sprintf(sql, "SELECT humi, temp FROM humitemp;");
    
    rc = sqlite3_exec(db, sql, 
            database_humitemp_retrieve_callback, 
            0, &err_msg);

    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

static int database_humitemp_retrieve_callback(void *unused, int count, char **data, char **columns)
{
    // TODO
    return 0;
}
