#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "list.h"

#define FOREACH_MUSIC   ListElmt *head=list->head; \
                        for(;head != NULL;head=head->next){

#define END_FOREACH_MUSIC }

#define PRINT_ERR fprintf(stderr, "%s[%d]: failed\n", \
                          __FUNCTION__, __LINE__)

pthread_mutex_t music_lock;
List *list;

void init_musics(void);
void deinit_musics(void);
void get_musics(void);
void add_music(char *name);
void remove_music(char *name);
void print_musics(void);

#endif
