#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "myhead.h"


int main(int argc, char *argv[]){

    key_t key;
    key = 10;


    char lenh = 0, g;  
    int lenhsend = 0; // для передачи в функции

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
            lenhsend = lenhsend + lenh;
        }
        lenh = 0;
    }

    client(key, argc / 3, mass_of_ukaz);
    server(key, argc / 3);
    return 0;

}
