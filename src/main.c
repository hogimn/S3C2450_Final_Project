#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include "list.h"
#include "music.h"
#include "itoa.h"
#include "devices.h"
#include "network.h"

#define PRINT_ERR fprintf(stderr, "%s[%d]: failed\n", \
                          __FUNCTION__, __LINE__)

#define BUF_SIZE 1024
#define TRUE 1
#define FALSE 0

void devices_init(void);
void create_devices_threads(void);

/* the functions socket_handler calls */
void receive_file(int sd);
void delete_file(int sd);
void play_music(int sd);
void transfer_list(int sd);
void transfer_sensor_data(int sd);
void transfer_camera_data(int sd);

/* periodic thread handler */
void *temphumi_handler(void *arg);
void *photo_handler(void *arg);
void *water_handler(void *arg);
void *magnetic_handler(void *arg);
void *moisture_handler(void *arg);

/* event-driven thread handler */
void *socket_handler(void *arg);
void *madplay(void *arg);

char current_music[BUF_SIZE];
pthread_mutex_t lock;

int main(int argc, char **argv)
{
    int sd, new_sd, port;
    /* thread to handle socket connection */
    pthread_t t_socket;

    /* initialize lock */
    pthread_mutex_init(&lock, 0);
    
    /* devices init */
    devices_init();

    /*
     * initialize data structure of music and 
     * get the list of music in from the directory "./music/"
     */
    music_init();

    /* create threads to controll devices */
    create_devices_threads();

    /* get port number if argc > 1 */
    network_get_port(argc, argv, &port);
    /* create socket, bind and listen */ 
    sd = network_server_init(port);
    if (sd == -1)
    {
        PRINT_ERR;
        exit(1);
    }

    /*
     * whenever client tries to connect,
     * create a new thread which will take the workload
     */
    while(1)
    {
        new_sd = network_accept_client(sd);
        if (new_sd < 0)
        {
            PRINT_ERR;
            exit(1);
        }

        /* create thread to handle in background */
        pthread_create(&t_socket, (void *)0, 
                       socket_handler, (void *)&new_sd);
    }

    return 0;
}

void create_devices_threads(void)
{
    /* thread to read temparature / humidity */
    pthread_t t_temphumi;
    /* thread to detect lack of photo intensity */
    pthread_t t_photo;
    /* thread to give water every predetermined interval */
    pthread_t t_water;
    /* thread to detect magnetic change */
    pthread_t t_magnetic;

    /* periodically loop */
    pthread_create(&t_temphumi, (void *)NULL, 
        temphumi_handler, (void *)NULL);

    /* periodically loop */
    pthread_create(&t_photo, (void *)NULL, 
        photo_handler, (void *)NULL);

    /* periodically loop */
    pthread_create(&t_water, (void *)NULL, 
        water_handler, (void *)NULL);

    /* periodically loop */
    pthread_create(&t_magnetic, (void *)NULL, 
        magnetic_handler, (void *)NULL);
}

void *temphumi_handler(void *arg)
{
    /* measure temparature/humidity every 1 sec */
    while (1)
    {
        // TODO
        printf("temphumi_handler\n"); 
        sleep(1);
    }

    printf("temphumi handler exited\n");
    pthread_exit(0);
}

void *photo_handler(void *arg)
{
    /* measure photo intensity every 1 sec */
    while (1)
    {
        // TODO
        printf("photo_handler\n"); 
        sleep(1); 
    }

    printf("photo handler exited\n");
    pthread_exit(0);
}

void *magnetic_handler(void *arg)
{
    /* mesaure magnet every 1 sec */
    while (1)
    {
        // TODO
        printf("magnetic_handler\n");
        sleep(1); 
    }

    printf("magnetic handler exited\n");
    pthread_exit(0);
}

void *water_handler(void *arg)
{
    /* give water at specific time */
    while (1)
    {
         
    }

    printf("water handler exited\n");
    pthread_exit(0);
}

void *socket_handler(void *arg)
{
    int sd;
    char buf[BUF_SIZE];
    int bytes_read;

    sd = *(int *)arg;

    bytes_read = recv(sd, (void *)buf, BUF_SIZE, 0);
    buf[bytes_read] = '\0';

    printf("received command: %s\n", buf);

    /* command 0: receive file from client */
    if (!strcmp(buf, SOCK_CMD_FILE_CLIENT_TO_SERVER))
    {
        receive_file(sd);
    }
    /* command 1: transfer music list to client */
    else if (!strcmp(buf, SOCK_CMD_LIST_SERVER_TO_CLIENT))
    {
        transfer_list(sd);
    }
    /* command 2: delete file */
    else if (!strcmp(buf, SOCK_CMD_DELETE_CLIENT_TO_SERVER))
    {
        delete_file(sd);
    }
    /* command 3: play the music */
    else if (!strcmp(buf, SOCK_CMD_PLAY_CLIENT_TO_SERVER))
    {
        play_music(sd);
    }
    /* command 4: transfer sensor data to client */
    else if (!strcmp(buf, SOCK_CMD_SENSOR_SERVER_TO_CLIENT))
    {
        transfer_sensor_data(sd);
    }
    /* command 5: transfer camera data to client */
    else if (!strcmp(buf, SOCK_CMD_CAMERA_SERVER_TO_CLIENT))
    {
        transfer_camera_data(sd);
    }

    printf("thread exit\n");
    pthread_exit(0);
}

void transfer_camera_data(int sd)
{
    int height = 144;
    int width = 176;
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
        if (-1 == send(sd, src_image, height*width*3, 0))
        {
            printf("connection failed\n");
            break; 
        }
    }

    free_video_capture();
    free_framebuffer();
    
    printf("camera data transfer exited");
}

void transfer_sensor_data(int sd)
{
    char buf[BUF_SIZE];
    int humitemp[2];
    char str_humi[3];
    char str_temp[3];
    int rc;

    /* loop while client is connected */
    while (1)
    {
        rc = temphumid_read(humitemp); 
        if (rc != TEMPHUMID_OK)
        {
            sleep(1);
            continue;
        }

        /* humidity, temparature integer to string */
        itoa(humitemp[0], str_humi);
        itoa(humitemp[1], str_temp);

        /* fill the buffer to send to client */
        sprintf(buf, "%s\n%s\n", str_humi, str_temp);
        printf("%s", buf);

        /* 
         * if client is disconnected, SIGPIPE will be ignored
         * and send will return -1 
         */
        if (-1 == send(sd, buf, strlen(buf), 0))
        {
            printf("connection failed\n");
            break; 
        }

        sleep(1);
    }
    
    printf("sensor data transfer exited");
}

void play_music(int sd)
{
    char buf[BUF_SIZE];
    char filename[BUF_SIZE];
    int bytes_read;
    pthread_t thread_madplay;

    /* receive file name */
    bytes_read = recv(sd, (void *)buf, BUF_SIZE, 0);
    buf[bytes_read] = '\0';

    strcpy(filename, buf);

    printf("playing music...\n");

    system("killall madplay");

    /* current_music != filename */
    if (strcmp(current_music, filename))
    {
        strcpy(current_music, filename);

        pthread_create(&thread_madplay, 
                       (void *)0,
                       madplay,
                       (void *)0);
    }
    else
    {
        bzero(current_music, sizeof(current_music));
        printf("music stoped\n");
    }
}

void *madplay(void *arg)
{
    char buf[BUF_SIZE];

    sleep(1);
    sprintf(buf, "madplay -a-8 -r -R 20000 \"./music/%s\"", current_music);
    system(buf);
    
    pthread_exit(0);
}

void delete_file(int sd)
{
    char buf[BUF_SIZE];
    char filename[BUF_SIZE];

    int bytes_read;

    /* receive file name */
    bytes_read = recv(sd, (void *)buf, BUF_SIZE, 0);
    buf[bytes_read] = '\0';

    /* store it to filename */
    strcpy(filename, buf);

    /* can't delete the file currently played */
    if (!strcmp(filename, current_music))
    {
        printf("can't delete the file currently played\n");
        return;
    }

    music_remove(filename);
    printf("delete completed\n");
}

void transfer_list(int sd)
{
    char buf[BUF_SIZE];
    
    /* lock function boundary */
    pthread_mutex_lock(&lock);

    /* 
     * traverse music data structure 
     * and send each title to client
     */
    FOREACH_MUSIC
        sprintf(buf, "%s\n", (const char *)head->data); 
        send(sd, buf, strlen(buf), 0);
        printf("trasnferring... %s", buf);
    END_FOREACH_MUSIC

    /* notify to client that transfer is over */
    sprintf(buf, "%s\n", (const char *)SOCK_CMD_END);
    send(sd, buf, strlen(buf), 0);

    /* send currently played music name */
    if (strcmp(current_music, ""))
    {
        sprintf(buf, "%s\n", current_music);
        send(sd, buf, strlen(buf), 0);
    }
    /* notify to client that transfer is over */
    sprintf(buf, "%s\n", (const char *)SOCK_CMD_END);
    send(sd, buf, strlen(buf), 0);

    printf("music list transfer completed\n");

    /* unlock function boundary */
    pthread_mutex_unlock(&lock);
}

void receive_file(int sd)
{
    char buf[BUF_SIZE];
    char filename[BUF_SIZE];

    int fd;
    int bytes_read, bytes_written;

    /* lock function boundary */
    pthread_mutex_lock(&lock);

    /* receive file name */
    bytes_read = recv(sd, (void *)buf, BUF_SIZE, 0);
    buf[bytes_read] = '\0';

    /* store it to filename */
    strcpy(filename, buf);

    /* change directory to "./music/" and create file with the name received */
    mkdir("./music/", 0755);
    chdir("./music/");

    fd = open(filename, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    if (fd == -1)
    {
        PRINT_ERR;
        exit(-1);
    }

    sleep(1);

    while (1)
    {
        /* read data from client */
        bytes_read = recv(sd, (void *)buf, BUF_SIZE, 0);
        /* when client socket is closed */
        if (bytes_read == 0) 
        {
            printf("break!\n");
            break; 
        }
        /* error */
        else if (bytes_read == -1) 
        {
            PRINT_ERR;
            exit(1);
        }

        /* write to the file created */
        bytes_written = write(fd, (void *)buf, bytes_read);
        if (bytes_written == -1)
        {
            PRINT_ERR;
            exit(1);
        }
    }

    chdir("..");

    /* add newly downloaded music to music list */
    music_add(filename);
    music_print();

    sprintf(buf, "%s\n", (const char *)SOCK_CMD_END);
    send(sd, buf, strlen(buf), 0);
    close(fd);

    printf("file received completed\n");

    /* unlock function boundary */
    pthread_mutex_unlock(&lock);
}

/* pheriperal devices are initialized here */
void devices_init(void)
{
    int rc;

    rc = relay_init();
    if (rc != RELAY_INIT_OK)
    {
        PRINT_ERR;
        exit(1);
    }

    rc = servo_init();
    if (rc != SERVO_INIT_OK)
    {
        PRINT_ERR;
        exit(1);
    }

    rc = moisture_init();
    if (rc != MOISTURE_INIT_OK)
    {
        PRINT_ERR;
        exit(1);
    }

    rc = temphumid_init();
    if (rc != TEMPHUMID_INIT_OK)
    {
        PRINT_ERR;
        exit(1);
    }

    rc = photo_init();
    if (rc != PHOTO_INIT_OK)
    {
        if (rc == PHOTO_INIT_FILE_OPEN_FAIL)
        {
            PRINT_ERR;
        }
        if (rc == PHOTO_INIT_I2C_FAIL)
        {
            PRINT_ERR;
        }
        exit(1);
    }

    rc = magnetic_init();
    if (rc != MAGNETIC_INIT_OK)
    {
        PRINT_ERR;
        exit(1);
    }

    rc = fan_init(); 
    if (rc != FAN_INIT_OK)
    {
        PRINT_ERR;
        exit(1);
    }

    /* do nothing at this moment */
    led_init();
    solenoid_init();
}

