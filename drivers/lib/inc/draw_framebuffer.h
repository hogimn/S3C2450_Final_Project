#ifndef DRAW_FRAMEBUFFER_H_
#define DRAW_FRAMEBUFFER_H_

void init_framebuffer();
void draw_framebuffer(unsigned char* src, int width, int height);
void free_framebuffer();
void clear_framebuffer();
unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b);

#endif
