#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h> 
#include<netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT     8080 
#define MAXLINE 1024 


char buf[MAXLINE];
char bufup[MAXLINE];

void *server(void* args);

int main(int argc, char *argv[])
{

    if (!(((argc - 1) % 3) == 0) || (argc < 3)) {
        printf("Usage: file textfile1 textfile2 spaces_to_delete ...\n");
        exit(-1);
    }

    pthread_t thread;
    int status;

    status = pthread_create(&thread, NULL, server, NULL);
    if (status != 0) {
        printf("main error: can't create thread, status = %d\n", status);
        exit(-2);
    }

    usleep(100000); 

    struct sockaddr_in     servaddr; 
    int udp_socket = -1;

    char lenh = 0, g;  
    int lenhsend = 0; // для передачи в функции

    // Creating socket file descriptor 
    if ( (udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    inet_aton("127.0.0.1" , &servaddr.sin_addr); 

    int len = sizeof(buf);



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
        memset(&mass_of_ukaz[g][lenh], 0, 30 - lenh);
        lenh = 0;
    }

    for (int i = 0; i < argc / 3; i++)
    {
        printf("Client translate...\n");
        sendto(udp_socket, (const char *)mass_of_ukaz[i], strlen(mass_of_ukaz[i]), 
            0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 


        int bytes_read = recvfrom(udp_socket, (char *)bufup, MAXLINE,  
                            0, (struct sockaddr *) &servaddr, 
                            &len); 
        
        printf("Client>recive %s\n", bufup);
    }
    

    close(udp_socket);
    return 0;

}