#ifndef myhead
#define myhead

#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

struct mem
{
    pthread_mutex_t	mutex;
    int NumR;
    int *lenght;
    char **mass;
} general;


void *server(void* num);

#endif