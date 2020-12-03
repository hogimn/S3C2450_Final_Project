/*
 * draw_framebuffer.c
 *
 *  Created on: Dec 24, 2015
 *      Author: Lincoln
 *
 * Modified: 11/2020 by Hoki Min
 */
#include "draw_framebuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>

static int fd = -1;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
//static char* fbp = NULL;
static short* fbp = NULL; /* 1 pixel for 2 bytes in the case of MDS2450 board */
static unsigned int screensize = 0;

void free_framebuffer(void)
{
    munmap(fbp, screensize);
    close(fd);
}

void init_framebuffer(void)
{
    fd = open("/dev/fb0", O_RDWR);
    if (fd == -1) 
    {
        perror("Error opening framebuffer device");
        exit(EXIT_FAILURE);
    }
    // Get fixed screen information
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) 
    {
        perror("Error reading fixed information");
        exit(EXIT_FAILURE);
    }
    // Get variable screen information
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1) 
    {
        perror("Error reading variable information");
        exit(EXIT_FAILURE);
    }
    screensize = (vinfo.xres * vinfo.yres * vinfo.bits_per_pixel) >> 3; /* (>>3): bits to bytes */
    screensize *= 2; /* to prevent segmentation fault */

    printf("xoffset: %d, yoffset: %d\nxres: %d, yres: %d\nbits_per_pixel: %d, line_length: %d\n",
            vinfo.xoffset, vinfo.yoffset, vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.line_length);
    /*
       vinfo.xres = 800;
       vinfo.yres = 600;
       if (ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo) == -1)
       {
       perror("Error puting variable information");
       exit(EXIT_FAILURE);
       }
       screensize = finfo.line_length * vinfo.yres;

       printf("xoffset: %d, yoffset: %d\nxres: %d, yres: %d\nbits_per_pixel: %d, line_length: %d\n",
       vinfo.xoffset, vinfo.yoffset, vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.line_length);
     */
    fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fbp == (short *)-1) 
    {
        perror("Error mapping framebuffer device to memory");
        exit(EXIT_FAILURE);
    }
}

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b)
{
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

void clear_framebuffer(void)
{
    /* clear framebuffer to black(0,0,0) */
    int x, y;
    int offset;
    for(y = 0; y < vinfo.yres; y++)
    {
        offset = y * vinfo.xres;
        for(x = 0; x < vinfo.xres; x++)
        {
            *(fbp+offset+x) = makepixel(0,0,0);
        }
    }
}

void draw_framebuffer(unsigned char* src, int width, int height)
{
    int x, y;
    unsigned int location = 0;
    int i = 0;

    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel >> 3) + (y + vinfo.yoffset) * finfo.line_length;
#if 0
            *(fbp + location) = src[i*3];           //B
            *(fbp + location + 1) = src[i*3 + 1];   //G
            *(fbp + location + 2) = src[i*3 + 2];   //R
#endif
#if 1
            unsigned short pixel = makepixel(src[i*3], src[i*3+1], src[i*3+2]); 
            *(fbp + location) = pixel;
#endif
            i++;
        }
    }
}
