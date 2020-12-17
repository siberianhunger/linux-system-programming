#ifndef myhead
#define myhead

#include <sys/types.h>
#include <stdio.h>

int client(key_t key, int n, char massive[][30]);
int server(key_t key, int n);

#endif