#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "list.h"

#define FOREACH_MUSIC   ListElmt *head=list->head; \
                        for(;head != NULL;head=head->next){

#define END_FOREACH_MUSIC }

pthread_mutex_t music_lock;
List *list;

void music_init(void);
void music_deinit(void);
void music_get_from_directory(void);
void music_add(char *name);
void music_remove(char *name);
void music_print(void);

#endif
