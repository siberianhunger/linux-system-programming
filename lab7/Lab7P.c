#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "myhead.h"

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS               0

int main(int argc, char const *argv[])
{
    pthread_t threads[argc / 3];
    int status;
    general.NumR = 0;
    pthread_mutex_init(&general.mutex, NULL);
    general.lenght = (int*)malloc(argc / 3 * sizeof(int));
    general.mass = (char**)malloc(argc / 3 * sizeof(char));
    long unsigned res;
    for (int i = 0; i < argc / 3; i++)
    {
        general.mass[i] = (char*)malloc(30 * sizeof(char));
        memset(general.mass[i], ' ', 30);
    }
    
    int lenh = 0, g;  

    if (!(((argc - 1) % 3) == 0) || (argc < 3)) {
        printf("Usage: file textfile1 textfile2 spaces_to_delete ...\n");
        exit(-1);
    }
    int lenargvs[(argc / 3) + 1];
    lenargvs[0] = 0;
    char mass_of_ukaz[argc / 3][30];   // массив указателей на начало новых 3х входных параметров
    for (g = 0; g < argc / 3; g++) // 
    {
        for (int i = 1; i < 4; i++)  // копирование входных переменных в строку.
        {
            strncpy(&mass_of_ukaz[g][lenh], argv[i + (g * 3)], strlen(argv[i + (g * 3)]));
            strncpy(&mass_of_ukaz[g][strlen(argv[i + (g * 3)]) + lenh], " ", 1);
            lenh = strlen(argv[i + (g * 3)]) + lenh + 1;
            
        }
        memset(&mass_of_ukaz[g][lenh], 0, 30 - lenh);
        general.mass[g] = mass_of_ukaz[g];
        general.lenght[g] = lenh - 1;
        lenh = 0;
    }
    printf("------------\n");
    for (int i = 0; i < argc / 3; i++)
    {
        status = pthread_create(&threads[i], NULL, server, &i);
        if (status != SUCCESS) {
            printf("main error: can't create thread, status = %d\n", status);
            exit(ERROR_CREATE_THREAD);
        }
    }
    for (int i = 0; i < argc / 3; i++)
    {
        status = pthread_join(threads[i], (void**)&res);
        if (status != SUCCESS) {
            printf("main error: can't join thread, status = %d\n", status);
            exit(ERROR_JOIN_THREAD);
        }
        printf("joined with address %ld\n", res);
    }
    
    //printf("first pack: %s \nlenght: %d \n", general.mass[0], general.lenght[0]);

}
