#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "network.h"
#include "error.h"

#define DATABASE_SQL_SIZE 128

static int database_data_socket_transfer_callback(void *unused, int count, char **data, char **columns);

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
        humi, temp);
    
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

void database_water_insert(int water)
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

void database_data_socket_transfer(int sd, int sensor)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];

    if (sensor == DATABASE_HUMI)
    {
        sprintf(sql, "SELECT humi FROM humitemp;");
    }
    else if (sensor == DATABASE_TEMP)
    {
        sprintf(sql, "SELECT temp FROM humitemp;");
        printf("select temp");
    }
    else {
        return;
    }
    
    rc = sqlite3_exec(db, sql, 
            database_data_socket_transfer_callback, 
            (void *)sd, &err_msg);

    if (rc != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        printf("%s\n", err_msg);
        sqlite3_close(db);
        ERR_HANDLE;
    }
}

static int database_data_socket_transfer_callback
    (void *arg, int count, char **data, char **columns)
{
    int sd;
    char buf[NETWORK_BUFSIZE];

    sd = (int)arg;

    sprintf(buf, "%s\n", data[0]);

    if (-1 == network_send(sd, buf, strlen(buf)))
    {
        printf("connection failed\n");
    }

    return 0;
}
