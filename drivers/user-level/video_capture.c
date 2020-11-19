/*
 * video_capture.c
 *
 *  Created on: Dec 24, 2015
 *      Author: Lincoln
 * 
 * Modified: 11/2020 by Hoki Min
 */
#include "video_capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>              /* low-level i/o control (open)*/
#include <errno.h>
#include <string.h>             /* strerror show errno meaning */
#include <sys/stat.h>           /* getting information about files attributes */
#include <linux/videodev2.h>    /* v4l2 structure */
#include <sys/mman.h>           /* memory mapping */
#include <unistd.h>             /* read write close */
#include <sys/time.h>           /* for select time */
#include <limits.h>             /* for UCHAR_MAX */


static char* dev_name = "/dev/video0";
static int fd = -1; /* vidoe0 file descriptor*/

/* Queried image buffers! */
struct buffer* buffers = NULL;
static unsigned int n_buffers = 0;
struct v4l2_buffer buf_in_while_loop;
static fd_set fds;
struct timeval tv;

/* wrapped errno display function by v4l2 API */
static void errno_exit(const char * s) {
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

/* wrapped ioctrl function by v4l2 API */
static int xioctl(int fd, int request, void * arg) {
    int r;
    do{
        r = ioctl(fd, request, arg);
    }
    while(-1 == r && EINTR == errno);
    return r;
}

static void open_device() {
    struct stat st;
    if (-1 == stat(dev_name, &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }
    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/*
 * 1. Memory mapped buffers are located in device memory and must be allocated with this ioctl
 *    before they can be mapped into the application's address space
 * 2. set four images in buffer
 */
static void init_mmap() {
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    /* Initiate Memory Mapping */
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support memory mapping\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }
    /* video output requires at least two buffers, one displayed and one filled by the application */
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
        exit(EXIT_FAILURE);
    }
    buffers = calloc(req.count, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        /* Query the status of a buffer */
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)){
            errno_exit("VIDIOC_QUERYBUF");
        }
        /* 176 * 144 * 2 = 50688 for MDS2450 board */
        buffers[n_buffers].length = buf.length; 
        printf("buffers[%d]: buf.length = %d\n", n_buffers, buf.length);

        buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length,
                PROT_READ | PROT_WRITE /* required */,
                MAP_SHARED /* recommended */, fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start){
            errno_exit("mmap");
        }
    }
}

/* set video streaming format here(width, height, pixel format, cropping, scaling) */
static void init_device(int width, int height) {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    unsigned int min;
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n", dev_name);
        exit(EXIT_FAILURE);
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
        exit(EXIT_FAILURE);
    }
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
        errno_exit("VIDIOC_S_FMT");
    }
    /* YUYV sampling 4 2 2, so bytes per pixel is 2*/
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min){
        fmt.fmt.pix.bytesperline = min;
    }
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min){
        fmt.fmt.pix.sizeimage = min;
    }
    init_mmap();
}

static void start_capturing() {
    unsigned int i;
    enum v4l2_buf_type type;
    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        /* enqueue an empty (capturing) or filled (output) buffer in the driver's incoming queue */
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
            errno_exit("VIDIOC_QBUF");
        }
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /* Start streaming I/O */
    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)){
        errno_exit("VIDIOC_STREAMON");
    }
}

static void close_device() {
    if (-1 == close(fd))
        errno_exit("close");
    fd = -1;
}

static void stop_capturing() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)){
        errno_exit("VIDIOC_STREAMOFF");
    }
}

static void uninit_device() {
    unsigned int i;
    for (i = 0; i < n_buffers; ++i){
        if (-1 == munmap(buffers[i].start, buffers[i].length)){
            errno_exit("munmap");
        }
    }
    free(buffers);
}

void yuv2rgb(int y, int u, int v, unsigned char *r, unsigned char *g, unsigned char *b)
{
    int r1, g1, b1;

    /* replaces floating point coefficients */
    int c = y-16, d = u - 128, e = v - 128;       

    /* Conversion that avoids floating point i*/
    r1 = (298 * c           + 409 * e + 128) >> 8;
    g1 = (298 * c - 100 * d - 208 * e + 128) >> 8;
    b1 = (298 * c + 516 * d           + 128) >> 8;

    /* Computed values may need clipping. */
    if (r1 > 255) r1 = 255;
    if (g1 > 255) g1 = 255;
    if (b1 > 255) b1 = 255;

    if (r1 < 0) r1 = 0;
    if (g1 < 0) g1 = 0;
    if (b1 < 0) b1 = 0;

    *r = r1 ;
    *g = g1 ;
    *b = b1 ;
}

#if 1
static void parse_im(const unsigned char *im_yuv, unsigned char *dst, int width, int height) {
    const int IM_SIZE = width * height;
    int i = 0, newi = 0;
    int y, u, y2, v;

    for(i = 0, newi = 0; i < 2*IM_SIZE; i=i+4, newi=newi+6){
        y=(int)im_yuv[i]; 
        u=(int)im_yuv[i+1]; 
        y2=(int)im_yuv[i+2]; 
        v=(int)im_yuv[i+3];

        yuv2rgb(y, u, v, &dst[newi], &dst[newi+1], &dst[newi+2]);
        yuv2rgb(y2, u, v, &dst[newi+3], &dst[newi+4], &dst[newi+5]);
    }

}
#endif

#if 0
static void parse_im(const unsigned char *im_yuv, unsigned char *dst, int width, int height) {
    const int IM_SIZE = width * height;
    unsigned char Y = 0;
    unsigned char U = 0;
    unsigned char V = 0;
    int B = 0;
    int G = 0;
    int R = 0;
    int i;
    for(i = 0; i < IM_SIZE; ++i){
        if(!(i & 1)){
            U = im_yuv[2 * i + 1];
            V = im_yuv[2 * i + 3];
        }
        Y = im_yuv[2 * i];
        yuv2rgb(Y, U, V, &R, &G, &B);
        B = Y + 1.773 * (U - 128);
        G = Y - 0.344 * (U - 128) - (0.714 * (V - 128));
        R = Y + 1.403 * (V - 128);
        if(B > UCHAR_MAX){
            B = UCHAR_MAX;
        }
        if(G > UCHAR_MAX){
            G = UCHAR_MAX;
        }
        if(R > UCHAR_MAX){
            R = UCHAR_MAX;
        }
        dst[3*i] = R;
        dst[3*i+1] = G;
        dst[3*i+2] = R;
    }
}
#endif

void init_video_capture(int width, int height){
    open_device();
    init_device(width, height);
    start_capturing();
}

char video_capture(unsigned char* dst, int width, int height){
    char key = 0;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    FD_SET(fileno(stdin), &fds);
    /* Timeout. */
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    select(fd + 1, &fds, NULL, NULL, &tv);

    if(FD_ISSET(fileno(stdin), &fds)){
        read(fileno(stdin), &key, 1);
        printf("key: %c\n", key);
        return key;
    }

    if(FD_ISSET(fd, &fds)){
        CLEAR(buf_in_while_loop);
        buf_in_while_loop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf_in_while_loop.memory = V4L2_MEMORY_MMAP;
        /* dequeue from buffer */
        if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf_in_while_loop)){
            switch(errno){
                case EAGAIN:
                    return 0;
                default:
                    errno_exit("VIDIOC_DQBUF");
            }
        }

        unsigned char* im_from_cam = (unsigned char*)buffers[buf_in_while_loop.index].start;
        parse_im(im_from_cam, dst, width, height);

        /* queue-in buffer */
        if(-1 == xioctl(fd, VIDIOC_QBUF, &buf_in_while_loop)){
            errno_exit("VIDIOC_QBUF");
        }
    }
    return 0;
}

void free_video_capture(){
    stop_capturing();
    uninit_device();
    close_device();
}
