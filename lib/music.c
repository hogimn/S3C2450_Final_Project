#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "music.h"

void init_musics(void)
{
    pthread_mutex_init(&music_lock, 0);

    list = (List *)malloc(sizeof(List));

    list_init(list, free);

    get_musics();
}

void deinit_musics(void)
{
    list_destroy(list); 
}

void get_musics(void)
{
    DIR *dir; 
    struct dirent *entry;
    int file_len;
    int file_len_without_dotmp3;
    char *ptr_substr;
    char *ptr_str; 

    /* open music directory */
    dir = opendir("./music/");
    if (dir == NULL)
    {
        /* no music entry */
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        ptr_substr = strstr(entry->d_name, ".mp3");
        ptr_str = entry->d_name;

        file_len = strlen(entry->d_name);
        file_len_without_dotmp3 = ptr_substr - ptr_str;

        /* file name ends with ".mp3" */
        if (ptr_substr &&
            file_len_without_dotmp3 == file_len-4) 
        {
            add_music(entry->d_name);
        }
    }

    closedir(dir);
}

void add_music(char *name)
{
    ListElmt *head = list->head;
    char *new_data;
    char *temp;

    for (; head != NULL; head=head->next)
    {
        temp = (char *)head->data;
        if (!strcmp(temp, name))
        {
            printf("already in list\n");
            return;
        }
    }

    /* allocate heap memory to new_data */
    new_data = (char *)malloc(strlen(name)+1);
    strcpy(new_data, name);

    /* critical section */
    pthread_mutex_lock(&music_lock);

    list_ins_next(list, list->tail, new_data);

    pthread_mutex_unlock(&music_lock);
}

void remove_music(char *name)
{
    int rc;
    char path[254];
    ListElmt *head = list->head;
    ListElmt *prehead = NULL;
    void *data_removed;
    
    sprintf(path, "./music/%s", name);

    /* delete physical file */
    rc = remove(path);
    if (rc == -1)
    {
        PRINT_ERR;        
    }

    /* remove from music list */
    for (; head != NULL; head=head->next)
    {
        if (!strcmp(name, (char *)head->data))
        {
            list_rem_next(list, prehead, &data_removed);
        }
        prehead = head;
    }

    free(data_removed);
}

void print_musics(void)
{
    ListElmt *head = list->head;

    for (; head != NULL; head=head->next)
    {
        printf("%s\n", (char *)head->data);
    }
}
