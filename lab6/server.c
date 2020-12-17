#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <stdlib.h>

#define MSGSZ     128
#define BUF_SIZE 1024


// получение аргрументов из строки (передаваемой пачки из 3х аргументов)
void get_arguments(int *spaces_to_delete, char *infile, char *outfile, char buff[MSGSZ], ssize_t n) {

    int p, trig = 0;
    buff[n+1] = '\0';    /* полное имя завершается 0 */
    *spaces_to_delete = buff[n];
    for (int i = 0; i < n; i++) {
        if (((int) buff[i] == ' ') && (trig == 0)) {
            strncpy(infile, buff, i);
            trig++;
            p = i;
        }

        if ((buff[i + 1] == ' ') && (trig == 1)) {
            memcpy(
                    outfile,
                    &buff[p + 1],
                    (int) (strchr(&buff[p + 1], ' ') - &buff[p + 1])
            );
            trig++;
            p = i;
        }

        if ((buff[i + 1] == ' ') && (trig == 2)) {
            *spaces_to_delete = atoi(&buff[i + 2]);
            break;
        }
    }
}

//чтение из файла и запись в другой с измененным содержимым
int write_in_file(char *infile, char *outfile, int spaces_to_delete) {
    int fd, ofd, openFlags;
    char buf[BUF_SIZE];
    ssize_t numRead;
    mode_t filePerms;
    fd = open(infile, O_RDONLY);
    if (fd == -1) {
        printf("Error opening file %s\n", infile);
        exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    ofd = open(outfile, openFlags, filePerms);
    if (ofd == -1) {
        printf("Error opening file %s\n ", outfile);
        exit(-3);
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



int server(key_t key, int n)
{
    int semid,shmid;
    struct sembuf sops[2];
    char *shmaddr, st = 0;
    char str[MSGSZ];


    if ((shmid = shmget(key, MSGSZ, 0666)) < 0) { perror("shmget"); return 1; }
    if ((shmaddr = (char*)shmat(shmid, NULL, 0)) == (void*)-1) { perror("shmat"); return 1; }

    semid = semget(key, 2, 0666);
    sops[0].sem_num = 0;
    sops[0].sem_flg = 0;
    sops[1].sem_num = 0;
    sops[1].sem_flg = 0;

    int spaces_to_delete;
    char infile[30] = "";
    char outfile[30] = "";
    int g;
    //открываем доступ клиенту
    printf("SERVER> Start\n");
    sops[1].sem_op = 1;
    semop(semid, &sops[1], 1);
    for (int i = 0; i < n; i++)
    {
        memset(infile, 0, sizeof(infile));
        memset(outfile, 0, sizeof(infile));
        g = 0;
        printf("SERVER> Waiting access to shared buffer\n");
        sops[0].sem_op = -1;
        semop(semid, &sops[0], 1);
        // Читаем строку
        strcpy(str, shmaddr);
        printf("SERVER> String: %s\n", str);
        for (int i = 0; i < 2; i++)
        {
            while (str[g] != ' ')
            {
                g++;
            }
            g++;
        }
        get_arguments(&spaces_to_delete, infile, outfile, str, g);
        //открываем доступ клиенту

        if (write_in_file(infile, outfile, spaces_to_delete) < 0){
            printf("error in write_in_file");
            return -1;
        }
        sops[1].sem_op = 1;
        semop(semid, &sops[1], 1);
    }
    shmdt(shmaddr);

}