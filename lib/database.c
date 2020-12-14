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
            humi(id INTEGER PRIMARY KEY, humi INT);\
        CREATE TABLE IF NOT EXISTS\
            temp(id INTEGER PRIMARY KEY, temp INT);\
         CREATE TABLE IF NOT EXISTS\
            photo(id INTEGER PRIMARY KEY, photo INT);\
         CREATE TABLE IF NOT EXISTS\
            magnet(id INTEGER PRIMARY KEY, magnet INT);\
         CREATE TABLE IF NOT EXISTS\
            moisture(id INTEGER PRIMARY KEY, moisture INT);";

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

void database_insert(int table, int data)
{
    int rc;
    char *err_msg;
    char sql[DATABASE_SQL_SIZE];

    switch (table)
    {
        case DATABASE_HUMI:
            sprintf(sql, "INSERT INTO humi (humi) VALUES (%d);", data);
            break;

        case DATABASE_TEMP:
            sprintf(sql, "INSERT INTO temp (temp) VALUES (%d);", data);
            break;

        case DATABASE_PHOTO:
            sprintf(sql, "INSERT INTO photo (photo) VALUES (%d);", data);
            break;

        case DATABASE_MAGNET:
            sprintf(sql, "INSERT INTO magnet (magnet) VALUES (%d);", data);
            break;

        case DATABASE_MOISTURE:
            sprintf(sql, "INSERT INTO moisture (moisture) VALUES (%d);", data);
            break;

        default:
            return;
    }

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
        sprintf(sql, "SELECT humi FROM humi;");
    }
    else if (sensor == DATABASE_TEMP)
    {
        sprintf(sql, "SELECT temp FROM temp;");
    }
    else if (sensor == DATABASE_PHOTO)
    {
        sprintf(sql, "SELECT photo FROM photo;");
    }
    else if (sensor == DATABASE_MAGNET)
    {
        sprintf(sql, "SELECT magnet FROM magnet;");
    }
    else if (sensor == DATABASE_MOISTURE)
    {
        sprintf(sql, "SELECT moisture FROM moisture;");
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
