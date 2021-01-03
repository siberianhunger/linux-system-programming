/*Программа сервера к лабораторной работе №4 */
/*2011 год*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h> 
#include<netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#define PORT     8080 
#define MAXLINE 1024 

#define BUF_SIZE 1024
#define MSGSZ     128

int udp_socket=-1; // идентификатор сокет для UDP
char udpbuf[1024]; // буфер для хранения данных
 
struct sockaddr_in addr_UDP; //Структура для хранения адреса

void init_UDP(void);// инициализация UDP сокет

void get_arguments(int* spaces_to_delete, char* infile, char* outfile, char buff[MSGSZ],ssize_t n){

    int p, trig = 0;
    buff[n + 1] = '\0';	/* полное имя завершается 0 */
    *spaces_to_delete = buff[n];
    for (int i = 0; i < n; i++)
    {
        if (((int)buff[i] == ' ') && (trig == 0)){
            strncpy(infile,buff,i);
            trig++;
            p = i;
        }

        if ((buff[i+1] == ' ') && (trig == 1))
        {
            memcpy(
                    outfile,
                    &buff[p + 1],
                    (int)(strchr(&buff[p + 1], ' ') - &buff[p + 1])
            );
            trig++;
            p = i;
        }

        if ((buff[i+1] == ' ') && (trig == 2))
        {
            *spaces_to_delete = atoi(&buff[i + 2]);
            break;
        }
    }
}

//чтение из файла и запись в другой с измененным содержимым
int write_in_file(char* infile, char* outfile, int spaces_to_delete){
    int fd, ofd, openFlags;
    char buf[BUF_SIZE];
    ssize_t numRead;
    mode_t filePerms;
    fd = open (infile, O_RDONLY);
    if (fd == -1)
    {
        printf ("Error opening file %s\n", infile);
        exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    ofd = open (outfile, openFlags, filePerms);
    if (ofd == -1)
    {
        printf ("Error opening file %s\n ", outfile) ; exit(-3);
    }

    /* Перемещение данных до достижения конца файла ввода или возникновения ошибки */

    char write_buf[BUF_SIZE];

    while ((numRead = read(fd, buf, BUF_SIZE)) > 0) {
        int count_spaces = 0;
        for (size_t i = 0; i < numRead; i++) {
            if (count_spaces < spaces_to_delete && buf[i] == ' ') {
                write_buf[i] = '\0';
                count_spaces++;
                printf("count_spaces ---- %d\n spaces_to_delete ---- %d\n ", count_spaces, spaces_to_delete);
            } else write_buf[i] = buf[i];

        }

        if (write(ofd, write_buf, numRead) != numRead) {
            printf("couldn't write whole buffer\n ");
            exit(-4);
        }
        if (numRead == -1) {
            printf("read error\n ");
            exit(-5);
        }
    }
    if (close(fd) == -1) {
        printf("close input error\n");
        exit(-6);
    }
    if (close(ofd) == -1) {
        printf("close output error\n");
        exit(-7);
    }

    close(fd);
    return 0;
}


void *server(void* args)
{
    int i;
    int recv;
    int structlen;
    char SENDER_ADDR[44];
    int byte1,byte2,byte3,byte4;
    unsigned char xorbyte=0;

    int spaces_to_delete;
    char infile[30] = "";
    char outfile[30] = "";
    memset(infile, 0, sizeof(infile));
    memset(outfile, 0, sizeof(infile));

    struct sockaddr_in src_addr_UDP; //Структура для хранения отправителя

    init_UDP(); // Инициализировали UDP сокет

    while(1) // Вечный цикл
    {
        memset(&src_addr_UDP, 0, sizeof(SENDER_ADDR));
        memset(udpbuf, 0, sizeof(udpbuf));
        structlen = sizeof(SENDER_ADDR);
        recv=recvfrom(udp_socket,(char *)udpbuf, 1024, MSG_WAITALL,(struct sockaddr *)&src_addr_UDP,&structlen);
        printf("Server> recv %d bytes: \n",recv); // напечатали сколько байт пришло


        byte1=((int)src_addr_UDP.sin_addr.s_addr&0xff000000)>>24;
        byte2=((int)src_addr_UDP.sin_addr.s_addr&0xff0000)>>16;
        byte3=((int)src_addr_UDP.sin_addr.s_addr&0xff00)>>8;
        byte4=((int)src_addr_UDP.sin_addr.s_addr&0xff);
        sprintf(SENDER_ADDR,"%d.%d.%d.%d",byte4,byte3,byte2,byte1);

        printf("Server> incumming in server -- %s\n", udpbuf);
        get_arguments(&spaces_to_delete, infile, outfile, udpbuf, recv - 1);
        if (write_in_file(infile, outfile, spaces_to_delete) < 0){
            printf("error in write_in_file");
        }
        sprintf(&udpbuf[recv]," --- Done");
        if(
            sendto(
                udp_socket,
                (char *)udpbuf,
                sizeof(udpbuf),
                MSG_CONFIRM,
                (const struct sockaddr *)&src_addr_UDP,
                sizeof(src_addr_UDP))
            == -1)
        {
            perror("UDP_SEND");
        }


    // ждем 0.1 секунду
        usleep(100000);
    }
}

void init_UDP(void) // инициализация сокет UDP
{

    struct sockaddr_in servaddr; // структура с типом адресов


     if ( (udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) //
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    // привязали сокет
    if ( bind(udp_socket, (const struct sockaddr *)&servaddr,
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

}