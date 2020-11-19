/*
============================================================================
Name        : live.c
Author      : LincolnHard
Version     :
Copyright   : free and open
Description : Hello World in C, Ansi-style
Revision    : Hoki Min (11/2020)
============================================================================
 */

#include "draw_framebuffer.h"
#include "video_capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* check the supported webcam resolutions using $v4l2-ctl --list-formats-ext */

int main(int argc, char** argv) 
{
    int width = 0, height = 0;

    if (argc >= 1) width = atoi(argv[0]);
    if (argc >= 2) height = atoi(argv[1]);

    /* 
     * MDS2450 board has USB 1.1 host,
     * It turns out that only less than or equal
     * to 176x144 resolution is supported.
     * So, in later code, the trick that 
     * enlarges the size of the screen to fit 
     * in LCD on MDS2450 is used.
     */
    if (width == 0) width = 176;
    if (height == 0) height = 144;

    printf("Using size: %dx%d.\n", width, height);
    usleep(1 * 1000000); // sleep one second

    unsigned char src_image[width * height * 3];
    init_framebuffer();
    init_video_capture(width, height);
    char key = 0;

    clear_framebuffer(width, height);
    for(; ;){
        key = video_capture(src_image, width, height);
        draw_framebuffer(src_image, width, height);
        if(key == 'q'){
            break;
        }
    }
    free_video_capture();
    free_framebuffer();
    return EXIT_SUCCESS;
}
