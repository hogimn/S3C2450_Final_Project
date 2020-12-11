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
#include <mqueue.h>
#include "list.h" 
#include "music.h"
#include "itoa.h"
#include "devices.h"
#include "network.h"
#include "error.h"

#define BUF_SIZE 1024
#define TRUE 1
#define FALSE 0

#define MQ_CMD_LED_ON           '1'
#define MQ_CMD_LED_OFF          '2'
#define MQ_CMD_FAN_ON           '3'
#define MQ_CMD_FAN_OFF          '4'
#define MQ_CMD_DRAIN            '5'
#define MQ_CMD_DRAIN_STOP       '6'
#define MQ_CMD_SOLENOID_OPEN    '7'
#define MQ_CMD_SOLENOID_CLOSE   '8'
#define MQ_CMD_HUMIDIFIER_ON   	'9'
#define MQ_CMD_HUMIDIFIER_OFF   '0'

void devices_init(void);
void create_polling_threads(void);
void message_queue_init(void);
void message_queue_handler(int mq_cmd);

/* the functions socket_handler calls */
void receive_file(int sd);
void delete_file(int sd);
void play_music(int sd);
void transfer_list(int sd);
void transfer_sensor_data(int sd);
void transfer_camera_data(int sd);

/* periodic thread handler */
void *humitemp_handler(void *arg);
void *photo_handler(void *arg);
void *water_handler(void *arg);
void *magnetic_handler(void *arg);
void *moisture_handler(void *arg);

/* event-driven thread handler */
void *socket_handler(void *arg);
void *madplay(void *arg);

/* contains the title of currently playing music */
char current_music[BUF_SIZE];

/* 
 * receive_file() and transfer_list()
 * will be atomically executed (function boundary lock)
 */
pthread_mutex_t recv_trans_lock;

/* message queue for handling request from sensors */
mqd_t mqd_main;

struct mq_attr attr = {
    .mq_maxmsg = 64, /* entire queue size: 64*4 bytes */
    .mq_msgsize = 4, /* 4 byte per message */
};

 /* flags to indicate the states of peripheral devices */
int flag_led;
int flag_fan;
int flag_solenoid;
int flag_drain;
int flag_humidifier;

/* 
 * global variables to store most recent sensor data
 * shared between {sensor name}_handler() (polling threads) (writer)
 * and transfer_sensor_data() (reader)
 * there is no race condition (no two writes at the same time)
 */
int temp;
int humi;
int photo;
int magnet;
int moisture;

int main(int argc, char **argv)
{
    int sd, new_sd, port;
    char mq_cmd;
    int rc;

    /* thread to handle socket connection */
    pthread_t t_socket;

    /* initialize lock */
    pthread_mutex_init(&recv_trans_lock, 0);

    /* devices init */
    devices_init();

    /*
     * initialize data structure of music and 
     * get the list of music in from the directory "./music/"
     */
    music_init();

    message_queue_init();

    /* create threads to measure sensor data via polling */
    create_polling_threads();

    /* get port number if argc > 1 */
    network_get_port(argc, argv, &port);

    /* create socket, bind and listen */ 
    sd = network_server_init(port);
    if (sd == -1)
    {
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
        rc = mq_receive(mqd_main, (char*)&mq_cmd, attr.mq_msgsize, 0);
        if (rc >= 0)
        {
            message_queue_handler(mq_cmd);
        }
    }

    return 0;
}

void message_queue_handler(int mq_cmd)
{
    switch (mq_cmd)
    {
        case MQ_CMD_LED_ON: 
            printf("MQ_CMD_LED_ON\n");
            flag_led=1;
            led_on();
            break;

        case MQ_CMD_LED_OFF: 
            printf("MQ_CMD_LED_OFF\n");
            flag_led=0;
            led_off();
            break;

        case MQ_CMD_FAN_ON: 
            printf("MQ_CMD_FAN_ON\n");
            flag_fan=1;
            fan_rotate(FAN_SPEED_FAST);
            break;

        case MQ_CMD_FAN_OFF: 
            printf("MQ_CMD_FAN_OFF\n");
            flag_fan=0;
            fan_off();
            break;

        case MQ_CMD_DRAIN: 
            printf("MQ_CMD_DRAIN\n");
            flag_drain = 1;
            servo_rotate(SERVO_180_DEGREE);
            break;

        case MQ_CMD_DRAIN_STOP: 
            printf("MQ_CMD_DRAIN_STOP\n");
            flag_drain = 0;
            servo_rotate(SERVO_0_DEGREE);
            break;

        case MQ_CMD_SOLENOID_OPEN: 
            printf("MQ_CMD_SOLENOID_OPEN\n");
            flag_solenoid=1;
            solenoid_open();
            break;

        case MQ_CMD_SOLENOID_CLOSE: 
            printf("MQ_CMD_SOLENOID_CLOSE\n");
            flag_solenoid=0;
            solenoid_close();
            break;
			
		case MQ_CMD_HUMIDIFIER_ON : 
			printf("MQ_CMD_HUMIDIFIER_ON\n");
			flag_humidifier=1;
			humidifier_on();
			break;
						
		case MQ_CMD_HUMIDIFIER_OFF : 
			printf("MQ_CMD_HUMIDIFIER_OFF\n");
			flag_humidifier=0;
			humidifier_off();
			break;

        default : 
            printf("invalid message queue command\n");
            break;
    }
}

void create_polling_threads(void)
{
    /*
     *#############################################
     *# threads to periodically measure something #
     *#############################################
     */

    /* thread to read temparature/humidity */
    pthread_t t_humitemp;
    /* thread to detect lack of photo intensity */
    pthread_t t_photo;
    /* thread to give water every predetermined interval */
    pthread_t t_water;
    /* thread to detect magnetic change */
    pthread_t t_magnetic;
    /* thread to measure soil moisture */
    pthread_t t_moisture;

    pthread_create(&t_humitemp, (void *)NULL, 
            humitemp_handler, (void *)NULL);

    pthread_create(&t_photo, (void *)NULL, 
            photo_handler, (void *)NULL);

    pthread_create(&t_water, (void *)NULL, 
            water_handler, (void *)NULL);

    pthread_create(&t_magnetic, (void *)NULL, 
            magnetic_handler, (void *)NULL);

    pthread_create(&t_moisture, (void *)NULL, 
            moisture_handler, (void *)NULL);
}

void *humitemp_handler(void *arg)
{
#if 1
    int rc;
    int humitemp[2];
    int humid_upper_fan = 80;
    int humid_under_fan = 70;
	int humid_upper_humidifier = 75;
	int humid_under_humidifier = 60;
    char cmd;

    /* initially fan is off state */
    flag_fan = 0;

    /* measure temp/humi every 1 sec */
    while (1)
    {
        rc = humitemp_read(humitemp);
        if (rc != HUMITEMP_READ_OK)
        {
            printf("humitemp_read() failed\n");
            sleep(1);
            continue;
        }

        /* write to global variable */
        humi = humitemp[0];
        temp = humitemp[1];

        printf("humi: %d, temp: %d\n", humi, temp);

#if 1   /* humi is outside of normal range and fan is off state */
		if (humi > humid_upper_fan && flag_fan==0)
        {
            printf("FAN ON\n");

            cmd = MQ_CMD_FAN_ON;
            mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
        }    
        /* humi gets less than predetermined bound and fan is on state */
        else if (humi < humid_under_fan && flag_fan==1)
        {
            printf("FAN OFF\n");

            cmd = MQ_CMD_FAN_OFF;
            mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
        }
#endif

#if 1 
		if(humi>humid_upper_humidifier&& flag_humidifier==1)
		{
			printf("humidifier_off\n");
			cmd = MQ_CMD_HUMIDIFIER_OFF;
			
			mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
		}	
		else if(humi<humid_under_humidifier&& flag_humidifier==0)
		{
			printf("humidifier_on\n");
			cmd = MQ_CMD_HUMIDIFIER_ON;
			
			mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
		}
#endif
        sleep(2);
    }

    printf("humitemp_handler() exited\n");
    pthread_exit(0);
#endif
}

void *photo_handler(void *arg)
{
#if 1
    int intensity_upper = 800;
    int intensity_under = 500;
    char cmd;
    
    /* initially led is off state */
    flag_led = 0;

    /* measure photo intensity every 1 sec */
    while (1)
    {
        photo = photo_get_intensity();
        if (photo == -1)
        {
            printf("photo_get_intensity() failed\n");
            sleep(1);
            continue;
        }
        
        printf("photo intensity: %d\n", photo); 
        
        /* if photo intensity gets higher than appropriate bound and LED is currently on state */
        if (photo > intensity_upper && flag_led==1)
        {
            printf("LED OFF\n");
            
            cmd = MQ_CMD_LED_OFF;
            mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
        }
        else if (photo < intensity_under && flag_led==0)
        {
            printf("LED ON\n");
            
            cmd = MQ_CMD_LED_ON;
            mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
        }
        
        sleep(1); 
    }

    printf("photo_handler() exited\n");
    pthread_exit(0);
# endif
}

void *magnetic_handler(void *arg)
{
#if 1
    char cmd;

    /* initially solenoid is open state */
    flag_solenoid = 1;
    
    /* measure magnet every 1 sec */
    while (1)
    {
        /* if magnet is detected and solenoid is open state */
        magnet = magnetic_is_detected();
        if (magnet && flag_solenoid==1)
        {
            printf("stop watering\n");
            
            cmd = MQ_CMD_SOLENOID_CLOSE;
            mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);
        }
        sleep(1); 
    }

    printf("magnetic_handler() exited\n");
    pthread_exit(0);
#endif
}

void *moisture_handler(void *arg)
{
#if 1
    char cmd;
    /* initially, drainage is open state */
    flag_drain = 0;
    
    /* measure soil moisture every 1 sec */
    while (1)
    {
        moisture = moisture_is_full();
        /* if moisture is full and drainage is close state */
        if (moisture && flag_drain==1)
        {
            printf("start draining\n");
            
            cmd = MQ_CMD_DRAIN;
            mq_send(mqd_main, (char*)&cmd, attr.mq_msgsize, 0);            
        }
        sleep(1);
    }

    printf("moisture_handler() exited\n");
    pthread_exit(0);
#endif
}

void *water_handler(void *arg)
{
#if 1
    /* give water at specific time */
    while (1)
    {
        /* 1. close drainage */

        /* 2. open solenoid */
    }

    printf("water_handler() exited\n");
    pthread_exit(0);
#endif
}

void *socket_handler(void *arg)
{
    int sd;
    char buf[BUF_SIZE];
    int bytes_read;

    sd = (int)arg;

    bytes_read = network_recv_poll(sd, (void *)buf, BUF_SIZE-1);
    buf[bytes_read] = '\0'; /* null terminated c-string */

    printf("received command: %s\n", buf);

    /* command 0: receive file from client */
    if (!strcmp(buf, SOCK_CMD_FILE_CLIENT_TO_SERVER))
    {
        receive_file(sd);
    } /* command 1: transfer music list to client */
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

    printf("socket_handler() exit\n");
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

    printf("transfer_camera_data() exited\n");
}

void transfer_sensor_data(int sd)
{
    char buf[BUF_SIZE];
    char str_humi[3];
    char str_temp[3];

    /* loop while client is connected */
    while (1)
    {
        if (temp <= 0 || humi <= 0)
        {
            printf("temp/humi is not set yet\n");
            sleep(1);
            continue;
        }

        /* humi, temp integer to string */
        itoa(humi, str_humi);
        itoa(temp, str_temp);

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

    printf("transfer_sensor_data() exited\n");
}

void play_music(int sd)
{
    char buf[BUF_SIZE];
    char filename[BUF_SIZE];
    int bytes_read;
    pthread_t thread_madplay;

    /* receive file name */
    bytes_read = network_recv_poll(sd, (void *)buf, BUF_SIZE-1);
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

        pthread_create(&thread_madplay, (void *)0,
                madplay, (void *)0);
    }
    /* clear current_music */
    else
    {
        bzero(current_music, sizeof(current_music));
        printf("music stoped\n");
    }

    printf("play_music() exited\n");
}

void *madplay(void *arg)
{
    char buf[BUF_SIZE];

    sleep(1);
    sprintf(buf, "madplay -a0 -r -R 20000 \"./music/%s\"", current_music);
    system(buf);

    printf("madplay() exited\n");
    pthread_exit(0);
}

void delete_file(int sd)
{
    char buf[BUF_SIZE];
    char filename[BUF_SIZE];

    int bytes_read;

    /* receive file name */
    bytes_read = network_recv_poll(sd, (void *)buf, BUF_SIZE-1);
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
    printf("delete_file() exited\n");
}

void transfer_list(int sd)
{
    char buf[BUF_SIZE];

    /* lock function boundary */
    pthread_mutex_lock(&recv_trans_lock);

    /* 
     * traverse music data structure 
     * and send each title to client
     */
    FOREACH_MUSIC
        sprintf(buf, "%s\n", (const char *)head->data); 
        send(sd, buf, strlen(buf), 0);
        printf("trasnferring... %s\n", buf);
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

    printf("transfer_list() exited\n");

    /* unlock function boundary lock */
    pthread_mutex_unlock(&recv_trans_lock);
}

void receive_file(int sd)
{
    char buf[BUF_SIZE];
    char filename[BUF_SIZE];

    int fd;
    int bytes_read, bytes_written;

    /* lock function boundary */
    pthread_mutex_lock(&recv_trans_lock);

    /* receive file name */
    bytes_read = network_recv_poll(sd, (void *)buf, BUF_SIZE-1);
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
        bytes_read = network_recv_poll(sd, (void *)buf, BUF_SIZE-1);
        /* when client socket is closed */
        if (bytes_read == 0) 
        {
            printf("break!\n");
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

    sprintf(buf, "%s\n", (const char *)SOCK_CMD_END);
    send(sd, buf, strlen(buf), 0);
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
