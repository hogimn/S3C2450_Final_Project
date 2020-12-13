#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include "list.h" 
#include "music.h"
#include "itoa.h"
#include "devices.h"
#include "network.h"
#include "database.h"
#include "error.h"

/* entire queue size: MQ_MAX_MSG * MQ_MSG_SIZE bytes */
#define MQ_MAX_MSG  64
/* 4 byte per message */
#define MQ_MSG_SIZE 4

/* message queue command */
#define MQ_CMD_LED_ON           1
#define MQ_CMD_LED_OFF          2
#define MQ_CMD_FAN_ON           3
#define MQ_CMD_FAN_OFF          4
#define MQ_CMD_DRAIN_OPEN       5
#define MQ_CMD_DRAIN_CLOSE      6
#define MQ_CMD_SOLENOID_OPEN    7
#define MQ_CMD_SOLENOID_CLOSE   8
#define MQ_CMD_HUMIDIFIER_ON    9
#define MQ_CMD_HUMIDIFIER_OFF   10
#define MQ_CMD_DRYER_ON         11
#define MQ_CMD_DRYER_OFF        12

/* state macro */
#define STATE_LED_ON            MQ_CMD_LED_ON
#define STATE_LED_OFF           MQ_CMD_LED_OFF
#define STATE_FAN_ON            MQ_CMD_FAN_ON
#define STATE_FAN_OFF           MQ_CMD_FAN_OFF
#define STATE_DRAIN_OPEN        MQ_CMD_DRAIN_OPEN
#define STATE_DRAIN_CLOSE       MQ_CMD_DRAIN_CLOSE
#define STATE_SOLENOID_OPEN     MQ_CMD_SOLENOID_OPEN
#define STATE_SOLENOID_CLOSE    MQ_CMD_SOLENOID_CLOSE
#define STATE_HUMIDIFIER_ON     MQ_CMD_HUMIDIFIER_ON
#define STATE_HUMIDIFIER_OFF    MQ_CMD_HUMIDIFIER_OFF
#define STATE_DRYER_ON          MQ_CMD_DRYER_ON
#define STATE_DRYER_OFF         MQ_CMD_DRYER_OFF

/* mode macro */
#define MODE_AUTO   0
#define MODE_MANUAL 1

void devices_init(void);
void states_init(void);
void create_polling_threads(void);
void message_queue_init(void);
void message_queue_handler(int mq_cmd);
void resources_deinit(int signum);
void drain_close(void);
void drain_open(void);

/* the functions socket_handler calls */
void receive_file(int sd);
void delete_file(int sd);
void play_music(int sd);
void transfer_list(int sd);
void transfer_sensor_data(int sd);
void transfer_camera_data(int sd);
void transfer_database_data(int sd);
void transfer_mode_data(int sd);
void transfer_state_data(int sd);
void mode_toggle(int sd);
void state_change(int sd);

/* periodic thread handler */
void *humitemp_handler(void *arg); /* every 2 sec */
void *photo_handler(void *arg);    /* every 1 sec */
void *water_handler(void *arg);    /* every 1 sec */
void *magnetic_handler(void *arg); /* every 1 sec */
void *moisture_handler(void *arg); /* every 1 sec */

/* event-driven thread handler */
void *socket_handler(void *arg);

/* contains the title of currently playing music */
char current_music[NETWORK_BUFSIZE];

/* 
 * receive_file() and transfer_list()
 * will be atomically executed (function boundary lock)
 */
pthread_mutex_t recv_trans_lock;

/* message queue for handling request from sensors */
mqd_t mqd_main;

/* database file name */
const char database_filename[] = "sensor.db";

struct mq_attr attr = {
    .mq_maxmsg = MQ_MAX_MSG,
    .mq_msgsize = MQ_MSG_SIZE,
};

/*
 *##################
 *# Sensor Devices #
 *##################
 */
 /* global variables to indicate the states of functional devices */
volatile int g_state_led;
volatile int g_state_fan;
volatile int g_state_solenoid;
volatile int g_state_drain;
volatile int g_state_humidifier;
volatile int g_state_dryer;

/*
 *######################
 *# Functional Devices #
 *######################
 */
/* 
 * global variables to store most recent sensor data
 * shared between {sensor name}_handler() (polling threads) (writer)
 * and transfer_sensor_data() (reader)
 */
volatile int g_temp;
volatile int g_humi;
volatile int g_photo;
volatile int g_magnet;
volatile int g_moisture;

/* global variable to indicate the mode of operation */
volatile int g_mode; /* MODE_AUTO is default */

int main(int argc, char **argv)
{
    int sd, new_sd, port;
    char mq_cmd[MQ_MSG_SIZE];
    int rc;

    /* override SIGINT handler */
    signal(SIGINT, resources_deinit);

    /* thread to handle socket connection */
    pthread_t t_socket;

    /* initialize lock */
    pthread_mutex_init(&recv_trans_lock, 0);

    /* devices init */
    devices_init();

    /* states init */
    states_init();

    /*
     * initialize data structure of music
     * and get the list of music from the directory "./music/"
     */
    music_init();

    /* 
     * open database with given file name 
     * and create necessary tables if they do not exist
     */
    database_init(database_filename);

    message_queue_init();

    /* create threads to measure sensor data periodically */
    create_polling_threads();

    /* get port number if argc > 1 */
    network_get_port(argc, argv, &port);

    /* create socket, bind and listen */ 
    sd = network_server_init(port);
    if (sd == -1) {
        ERR_HANDLE;
    }

    /* 
     * non-blocking polling starts 
     * 1. socket - accept()
     * 2. message queue - mq_receive()
     */
    while(1)
    {
        /*
         * non-blocking accept
         * whenever client tries to connect,
         * create a new thread which will take the workload
         */
        new_sd = network_accept_client(sd);
        if (new_sd >= 0)
        {
            pthread_create(&t_socket, (void *)0, 
                    socket_handler, (void *)new_sd);
        }

        /* non-blocking message queue wait */
        rc = mq_receive(mqd_main, (char*)mq_cmd, attr.mq_msgsize, 0);
        if (rc >= 0)
        {
            message_queue_handler(atoi(mq_cmd));
        }
    }

    return 0;
}

void message_queue_handler(int mq_cmd)
{
    switch (mq_cmd)
    {
        case MQ_CMD_LED_ON:
            led_on();
            g_state_led = STATE_LED_ON;

            printf("LED ON\n");
            break;

        case MQ_CMD_LED_OFF:
            led_off();
            g_state_led = STATE_LED_OFF;

            printf("LED OFF\n");
            break;

        case MQ_CMD_FAN_ON:
            fan_rotate(FAN_SPEED_FAST);
            g_state_fan = STATE_FAN_ON;

            printf("FAN ON\n");
            break;

        case MQ_CMD_FAN_OFF:
            fan_off();
            g_state_fan = STATE_FAN_OFF;

            printf("FAN OFF\n");
            break;

        case MQ_CMD_DRAIN_OPEN:
            drain_open();
            g_state_drain = STATE_DRAIN_OPEN;

            printf("DRAIN OPEN\n");
            break;

        case MQ_CMD_DRAIN_CLOSE:
            drain_close();
            g_state_drain = STATE_DRAIN_CLOSE;

            printf("DRAIN CLOSE\n");
            break;

        case MQ_CMD_SOLENOID_OPEN:
            solenoid_open();
            g_state_solenoid = STATE_SOLENOID_OPEN;

            printf("SOLENOID OPEN\n");
            break;

        case MQ_CMD_SOLENOID_CLOSE:
            solenoid_close();
            g_state_solenoid = STATE_SOLENOID_CLOSE;

            printf("SOLENOID CLOSE\n");
            break;

        case MQ_CMD_HUMIDIFIER_ON:
            humidifier_on();
            g_state_humidifier = STATE_HUMIDIFIER_ON;

            printf("HUMIDIFIER ON\n");
            break;

        case MQ_CMD_HUMIDIFIER_OFF:
            humidifier_off();
            g_state_humidifier = STATE_HUMIDIFIER_OFF;

            printf("HUMIDIFIER OFF\n");
            break;

        case MQ_CMD_DRYER_ON:
            dryer_on(); 
            g_state_dryer = STATE_DRYER_ON;

            printf("DRYER ON\n"); 
            break;

        case MQ_CMD_DRYER_OFF:
            dryer_on(); 
            g_state_dryer = STATE_DRYER_OFF;

            printf("DRYER OFF\n"); 
            break;

        default:
            printf("invalid message queue command\n");
            break;
    }
}

void *socket_handler(void *arg)
{
    int sd;
    char buf[NETWORK_BUFSIZE];
    int bytes_read;
    int cmd;

    sd = (int)arg;

    bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
    buf[bytes_read] = '\0'; /* null terminated c-string */

    cmd = atoi(buf);

    printf("received command: %s\n", buf);

    switch (cmd)
    {
        /* command 0: receive file from client */
        case NETWORK_CMD_FILE_CLIENT_TO_SERVER:
            receive_file(sd);
            break;

        /* command 1: transfer music list to client */
        case NETWORK_CMD_LIST_SERVER_TO_CLIENT:
            transfer_list(sd);
            break;

        /* command 2: delete file */
        case NETWORK_CMD_DELETE_CLIENT_TO_SERVER:
            delete_file(sd);
            break;

        /* command 3: play the music */
        case NETWORK_CMD_PLAY_CLIENT_TO_SERVER:
            play_music(sd);
            break;

        /* command 4: transfer sensor data to client */
        case NETWORK_CMD_SENSOR_SERVER_TO_CLIENT:
            transfer_sensor_data(sd);
            break;

        /* command 5: transfer camera data to client */
        case NETWORK_CMD_CAMERA_SERVER_TO_CLIENT:
            transfer_camera_data(sd);
            break;

        /* command 6: transfer database data to client */
        case NETWORK_CMD_DATABASE_SERVER_TO_CLIENT:
            transfer_database_data(sd);
            break;

        /* command 7: transfer mode data to client */
        case NETWORK_CMD_MODE_SERVER_TO_CLIENT:
            transfer_mode_data(sd); 
            break;

        /* command 8: toggle mode between MODE_AUTO and MODE_MANUAL */
        case NETWORK_CMD_MODE_TOGGLE_CLIENT_TO_SERVER:
            mode_toggle(sd);
            break;

        /* command 9: transfer state data to client */
        case NETWORK_CMD_STATE_SERVER_TO_CLIENT:
            transfer_state_data(sd);

        /* command 10: change device state */
        case NETWORK_CMD_STATE_CHANGE_CLIENT_TO_SERVER:
            state_change(sd);
            break;

        default:
            printf("invalide network command\n");
            break;
    }

    printf("socket_handler() exited\n");
    pthread_exit(0);
}

void create_polling_threads(void)
{
    /*
     *#############################################
     *# threads to periodically measure something #
     *#############################################
     */

    /* thread to watch temparature/humidity sensor */
    pthread_t t_humitemp;
    /* thread to watch photo intensity sensor */
    pthread_t t_photo;
    /* thread to watch magnetic sensor */
    pthread_t t_magnetic;
    /* thread to watch soil moisture sensor */
    pthread_t t_moisture;
    /* thread to give water every predetermined interval */
    pthread_t t_water;

    pthread_create(&t_humitemp, (void *)NULL, 
            humitemp_handler, (void *)NULL);

    pthread_create(&t_photo, (void *)NULL, 
            photo_handler, (void *)NULL);

    pthread_create(&t_magnetic, (void *)NULL, 
            magnetic_handler, (void *)NULL);

    pthread_create(&t_moisture, (void *)NULL, 
            moisture_handler, (void *)NULL);

    pthread_create(&t_water, (void *)NULL, 
            water_handler, (void *)NULL);
}

void *humitemp_handler(void *arg)
{
    int rc;
    int humitemp[2];
    int humid_upper_fan = 75;
    int humid_under_fan = 70;
    int humid_upper_humidifier = 65;
    int humid_under_humidifier = 60;
    int temp_upper_dryer = 25;
    int temp_under_dryer = 20;
    char cmd[MQ_MSG_SIZE];

    /* measure humi/temp every 1 sec */
    while (1)
    {
        rc = humitemp_read(humitemp);
        if (rc != HUMITEMP_READ_OK)
        {
            printf("humitemp_read() failed\n");
            sleep(2);
            continue;
        }

        /* write to global variable */
        g_humi = humitemp[0];
        g_temp = humitemp[1];

        printf("humi: %d, temp: %d\n", g_humi, g_temp);

        /* control functional devices only when mode is auto mode */
        if (g_mode == MODE_AUTO)
        {
            /*
             * humi is higher than upper limit 
             * and fan is off state
             */
            if (g_humi > humid_upper_fan && 
                    g_state_fan == STATE_FAN_OFF)
            {
                sprintf(cmd, "%d", MQ_CMD_FAN_ON);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }    
            /* 
             * humi gets less than predetermined bound 
             * and fan is on state 
             */
            else if (g_humi < humid_under_fan &&
                    g_state_fan == STATE_FAN_ON)
            {
                sprintf(cmd, "%d", MQ_CMD_FAN_OFF);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }

            /* 
             * humi is less than lower limit
             * and humidifer is off state 
             */
            if (g_humi < humid_under_humidifier &&
                    g_state_humidifier == STATE_HUMIDIFIER_OFF)
            {
                sprintf(cmd, "%d", MQ_CMD_HUMIDIFIER_ON);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
            /* 
             * humi gets higher than predetermined bound 
             * and humidifer is on state
             */
            else if (g_humi > humid_upper_humidifier && 
                    g_state_humidifier == STATE_HUMIDIFIER_ON)
            {
                sprintf(cmd, "%d", MQ_CMD_HUMIDIFIER_OFF);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }

            /* 
             * temp is less than lower limit
             * and dryer is off state 
             */
            if (g_temp < temp_under_dryer &&
                    g_state_dryer == STATE_DRYER_OFF)
            {
                sprintf(cmd, "%d", MQ_CMD_DRYER_ON);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
            /* 
             * temp gets higher than predetermined bound 
             * and dryer is on state
             */
            else if (g_temp > temp_upper_dryer && 
                    g_state_dryer == STATE_DRYER_ON)
            {
                sprintf(cmd, "%d", MQ_CMD_DRYER_OFF);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
        }

        /* store into database table "humitemp" */
        database_humitemp_insert(g_humi, g_temp);

        sleep(2);
    }

    printf("humitemp_handler() exited\n");
    pthread_exit(0);
}

void *photo_handler(void *arg)
{
    int intensity_upper = 800;
    int intensity_under = 500;
    char cmd[MQ_MSG_SIZE];
    
    /* measure photo intensity every 1 sec */
    while (1)
    {
        g_photo = photo_get_intensity();
        if (g_photo == -1)
        {
            printf("photo_get_intensity() failed\n");
            sleep(1);
            continue;
        }
        
        printf("photo intensity: %d\n", g_photo); 
        
        /* control functional devices only when mode is auto mode */
        if (g_mode == MODE_AUTO)
        { 
            /* 
             * if photo intensity is less than lower limit 
             * and LED is currently off state
             */
            if (g_photo < intensity_under && 
                    g_state_led == STATE_LED_OFF)
            {
                sprintf(cmd, "%d", MQ_CMD_LED_ON);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
            /* 
             * if photo intensity gets higher than appropriate bound 
             * and LED is currently on state
             */
            else if (g_photo > intensity_upper &&
                    g_state_led == STATE_LED_ON)
            {
                sprintf(cmd, "%d", MQ_CMD_LED_ON);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
        }
        
        sleep(1); 
    }

    printf("photo_handler() exited\n");
    pthread_exit(0);
}

void *magnetic_handler(void *arg)
{
    char cmd[MQ_MSG_SIZE];

    /* measure magnet every 1 sec */
    while (1)
    {
        /* if magnet is detected and solenoid is open state */
        g_magnet = mag_valid_detectection();

        /* control functional devices only when mode is auto mode */
        if (g_mode == MODE_AUTO)
        {
            if (g_magnet == MAGNETIC_DETECTED && 
                    g_state_solenoid == STATE_SOLENOID_OPEN)
            {
                sprintf(cmd, "%d", MQ_CMD_SOLENOID_CLOSE);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
        }

        sleep(1); 
    }

    printf("magnetic_handler() exited\n");
    pthread_exit(0);
}

void *moisture_handler(void *arg)
{
    char cmd[MQ_MSG_SIZE];

    /* measure soil moisture every 1 sec */
    while (1)
    {
        g_moisture = moisture_is_full();

        /* control functional devices only when mode is auto mode */
        if (g_mode == MODE_AUTO)
        {
            /* if moisture is full and drainage is close state */
            if (g_moisture == MOISTURE_FULL && 
                    g_state_drain == STATE_DRAIN_CLOSE)
            {
                sprintf(cmd, "%d", MQ_CMD_DRAIN_OPEN);
                mq_send(mqd_main, (char*)cmd, attr.mq_msgsize, 0);
            }
        }

        sleep(1);
    }

    printf("moisture_handler() exited\n");
    pthread_exit(0);
}

void *water_handler(void *arg)
{
    /* give water at specific time */
    // TODO
    {
        /* 1. close drainage */

        /* 2. open solenoid */
    }

    printf("water_handler() exited\n");
    pthread_exit(0);
}

void transfer_database_data(int sd)
{
    char buf[NETWORK_BUFSIZE];

    int sensor;
    int bytes_read;

    /* receive command */
    bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
    buf[bytes_read] = '\0';

    sensor = atoi(buf); 

    database_data_socket_transfer(sd, sensor);
}

void transfer_state_data(int sd)
{
    int rc;
    char buf[NETWORK_BUFSIZE];

    while (1)
    {
        sprintf(buf, "%d\n%d\n%d\n%d\n%d\n%d\n",
                g_state_led, g_state_fan, g_state_solenoid,
                g_state_drain, g_state_humidifier, g_state_dryer);

        rc = network_send(sd, buf, strlen(buf));
        if (rc == -1)
        {
            printf("client socket is closed\n");
            break;
        }
        sleep(1);
    }

    printf("transfer_state_data() exited\n");
}

void state_change(int sd)
{
    char buf[NETWORK_BUFSIZE];

    int cmd;
    int bytes_read;

    /* only available in MANUAL mode */
    if (g_mode != MODE_MANUAL)
    {
        return;
    }

    /* receive command */
    bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
    buf[bytes_read] = '\0';

    cmd = atoi(buf); 

    message_queue_handler(cmd);
}

void mode_toggle(int sd)
{
    g_mode ^= 1;
    transfer_mode_data(sd);
}

void transfer_mode_data(int sd)
{
    int rc;
    char buf[NETWORK_BUFSIZE];

    sprintf(buf, "%d\n", g_mode);
    rc = network_send(sd, buf, strlen(buf));
    if (rc == -1)
    {
        ERR_HANDLE;
    }

    printf("transfer_mode_data() exited\n");
}

void transfer_camera_data(int sd)
{
    int rc;
    const int height = 144;
    const int width = 176;
    unsigned char src_image[width*height*3];

    init_framebuffer();
    init_video_capture(width, height);

    /* loop while client is connected */
    while (1)
    {
        video_capture(src_image, width, height);

        /* 
         * if client is disconnected, SIGPIPE will be ignored
         * and send will return -1 
         */
        rc = network_send(sd, src_image, height*width*3);
        if (rc == -1)
        {
            printf("client socket is closed\n");
            break; 
        }
    }

    free_video_capture();
    free_framebuffer();

    printf("transfer_camera_data() exited\n");
}

void transfer_sensor_data(int sd)
{
    int rc;
    char buf[NETWORK_BUFSIZE];
    char str_humi[3];
    char str_temp[3];

    /* loop while client is connected */
    while (1)
    {
        if (g_humi <= 0 || g_temp <= 0)
        {
            printf("humi/temp is not set yet\n");
            sleep(1);
            continue;
        }

        /* humi, temp integer to string */
        itoa(g_humi, str_humi);
        itoa(g_temp, str_temp);

        /* fill the buffer to send to client */
        sprintf(buf, "%s\n%s\n", str_humi, str_temp);

        /* 
         * if client is disconnected, SIGPIPE will be ignored
         * and send will return -1 
         */
        rc = network_send(sd, buf, strlen(buf));
        if (rc == -1)
        {
            printf("client socket is closed\n");
            break; 
        }

        /* should be same amount as in humitemp_handler() */
        sleep(2);
    }

    printf("transfer_sensor_data() exited\n");
}

void play_music(int sd)
{
    char buf[NETWORK_BUFSIZE];
    char filename[NETWORK_BUFSIZE];
    int bytes_read;

    /* receive file name */
    bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
    buf[bytes_read] = '\0';

    strcpy(filename, buf);

    printf("playing music...\n");

    system("killall madplay");

    /* 
     * current_music != received_music 
     * start a new thread which plays received music
     */
    if (strcmp(current_music, filename))
    {
        strcpy(current_music, filename);
        sprintf(buf, "madplay -a-7 -r -R 20000 \"./music/%s\"", current_music);
        system(buf);
    }
    /* clear current_music */
    else
    {
        bzero(current_music, sizeof(current_music));
        printf("music stoped\n");
    }

    printf("play_music() exited\n");
}

void delete_file(int sd)
{
    char buf[NETWORK_BUFSIZE];
    char filename[NETWORK_BUFSIZE];

    int bytes_read;

    /* receive file name */
    bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
    buf[bytes_read] = '\0';

    /* store it to filename */
    strcpy(filename, buf);

    /* can't delete the file currently playing */
    if (!strcmp(filename, current_music))
    {
        printf("can't delete the file currently played\n");
        return;
    }

    music_remove(filename);
    printf("delete_file() exited\n");
}

void transfer_list(int sd)
{
    int rc;
    char buf[NETWORK_BUFSIZE];

    /* lock function boundary */
    pthread_mutex_lock(&recv_trans_lock);

    /* 
     * traverse music data structure 
     * and send each title to client
     */
    FOREACH_MUSIC
        sprintf(buf, "%s\n", (const char *)head->data);
        rc = network_send(sd, buf, strlen(buf));
        if (rc == -1)
        {
            ERR_HANDLE;
        }
        printf("trasnferring... %s\n", buf);
    END_FOREACH_MUSIC

    /* notify to client that transfer is over */
    rc = network_send_cmd_end(sd);
    if (rc == -1)
    {
        ERR_HANDLE;
    }
    if (rc == -1)
    {
        ERR_HANDLE;
    }

    /* send currently played music name */
    if (strcmp(current_music, ""))
    {
        sprintf(buf, "%s\n", current_music);
        rc = network_send(sd, buf, strlen(buf));
        if (rc == -1)
        {
            ERR_HANDLE;
        }
    }

    /* notify to client that transfer is over */
    rc = network_send_cmd_end(sd);
    if (rc == -1)
    {
        ERR_HANDLE;
    }

    printf("transfer_list() exited\n");

    /* unlock function boundary lock */
    pthread_mutex_unlock(&recv_trans_lock);
}

void receive_file(int sd)
{
    char buf[NETWORK_BUFSIZE];
    char filename[NETWORK_BUFSIZE];

    int fd;
    int rc;
    int bytes_read, bytes_written;

    /* lock function boundary */
    pthread_mutex_lock(&recv_trans_lock);

    /* receive file name */
    bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
    buf[bytes_read] = '\0';

    /* store it to filename */
    strcpy(filename, buf);

    /* change directory to "./music/" and create file with the name received */
    mkdir("./music/", 0755);
    chdir("./music/");

    fd = open(filename, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    if (fd == -1)
    {
        ERR_HANDLE;
    }

    sleep(1);

    while (1)
    {
        /* read data from client */
        bytes_read = network_recv(sd, (void *)buf, NETWORK_BUFSIZE-1);
        /* when client socket is closed */
        if (bytes_read == 0) 
        {
            printf("client socket is closed\n");
            break; 
        }
        /* error */
        else if (bytes_read == -1) 
        {
            ERR_HANDLE;
        }

        /* write to the file created */
        bytes_written = write(fd, (void *)buf, bytes_read);
        if (bytes_written == -1)
        {
            ERR_HANDLE;
        }
    }

    chdir("..");

    /* add newly downloaded music to music list */
    music_add(filename);
    music_print();

    rc = network_send_cmd_end(sd);
    if (rc == -1)
    {
        ERR_HANDLE;
    }

    close(fd);

    printf("receive_file() exited\n");

    /* unlock function boundary lock */
    pthread_mutex_unlock(&recv_trans_lock);
}

/* pheriperal devices are initialized here */
void devices_init(void)
{
    int rc;

    rc = relay_init();
    if (rc != RELAY_INIT_OK)
    {
        ERR_HANDLE;
    }

    rc = servo_init();
    if (rc != SERVO_INIT_OK)
    {
        ERR_HANDLE;
    }

    rc = moisture_init();
    if (rc != MOISTURE_INIT_OK)
    {
        ERR_HANDLE;
    }

    rc = humitemp_init();
    if (rc != HUMITEMP_INIT_OK)
    {
        ERR_HANDLE;
    }

    rc = photo_init();
    if (rc != PHOTO_INIT_OK)
    {
        if (rc == PHOTO_INIT_FILE_OPEN_FAIL)
        {
            ERR_HANDLE;
        }
        if (rc == PHOTO_INIT_I2C_FAIL)
        {
            ERR_HANDLE;
        }
    }

    rc = magnetic_init();
    if (rc != MAGNETIC_INIT_OK)
    {
        ERR_HANDLE;
    }

    rc = fan_init(); 
    if (rc != FAN_INIT_OK)
    {
        ERR_HANDLE;
    }
    
    rc = humidifier_init(); 
    if (rc != HUMIDIFIER_INIT_OK)
    {
        ERR_HANDLE;
    }

    /* do nothing at this moment */
    led_init();
    solenoid_init();
    dryer_init();
}

void devices_deinit(void)
{
    relay_deinit();
    servo_deinit();
    moisture_deinit();
    humitemp_deinit();
    photo_deinit();
    magnetic_deinit();
    fan_deinit();
    humidifier_deinit();
    led_deinit();
    solenoid_deinit();
    dryer_deinit(); 
}

void message_queue_init(void)
{
    /* create non-blocking message queue */
    mqd_main = mq_open("/mq_main", 
            O_CREAT|O_NONBLOCK|O_RDWR, 0666, &attr);
    if (mqd_main == -1)
    {
        ERR_HANDLE;
    }
}

void message_queue_deinit(void)
{
    int rc;

    rc = mq_close(mqd_main);
    if (rc == -1)
    {
        ERR_HANDLE;
    }
}

void resources_deinit(int signum)
{
    devices_deinit();
    music_deinit();
    database_deinit();
    message_queue_deinit();

    printf("resources successfully deinitialized\n");

    exit(1);
}

void drain_open(void)
{
    servo_rotate(SERVO_180_DEGREE);
}

void drain_close(void)
{
    servo_rotate(SERVO_0_DEGREE);
}

void states_init(void)
{
    g_state_led = STATE_LED_OFF;
    g_state_fan = STATE_FAN_OFF;
    g_state_solenoid = STATE_SOLENOID_CLOSE;
    g_state_drain = STATE_DRAIN_OPEN;
    g_state_humidifier = STATE_HUMIDIFIER_OFF;
    g_state_dryer = STATE_DRYER_OFF;
}
