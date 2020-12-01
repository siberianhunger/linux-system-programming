#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pipe_server.c"
#define MAXLINE 90

extern void pipe_server(int, int);

int main(int argc, char *argv[]) {
    // проверка на количество аргументов в командной строке
    if (argc < 4 || (argc - 1) % 3 != 0) {
        printf("Usage: spaces_to_delete inp_file1 out_file1 spaces_to_delete inp_file2 out_file2 ...\n");
        exit(-1);
    }

    int countInput = (argc - 1) / 3; // количество входных файлов
    int i, pid[countInput];
    int pipe1[countInput][2], pipe2[countInput][2];
    char buff[MAXLINE]; // строка для i-ой тройки аргументов
    char arg1[30], arg2[30], arg3[30]; // для трех аргуметов

    for (i = 0; i < countInput; i += 1) {
        /* создание пары каналов для i-го дочернего процесса */
        pipe(pipe1[i]);
        pipe(pipe2[i]);

        /* создание i-го дочернего процесса */
        pid[i] = fork();
        if (pid[i] == 0) {
            /* child */
            close(pipe1[i][1]);
            close(pipe2[i][0]);
            pipe_server(pipe1[i][0], pipe2[i][1]);
            exit(0);
        }
    }

    /* отправка i-ой строки с аргументами в канал IPC*/
    for (i = 0; i < countInput; i += 1) {
        strcpy(arg1, argv[i * 3 + 1]);
        strcpy(arg2, argv[i * 3 + 2]);
        strcpy(arg3, argv[i * 3 + 3]);
        sprintf(buff, "%s %s %s", arg1, arg2, arg3); // строка с i-ой тройкой аргументами без исполняемого файла

        /* запись строки с тройкой аргументов в канал IPC */
        write(pipe1[i][1], buff, strlen(buff));
    }

    sleep(1);

    /* ожидание окончания выполнения всех запущенных дочерних процессов */
    int n;
    for (i = 0; i < countInput; i += 1) {
        memset(buff, 0, MAXLINE);
        if (pid[i] == waitpid(pid[i], NULL, 0)) {
            if ((n = read(pipe2[i][0], buff, MAXLINE)) > 0) {
                /* считывание из канала, вывод в stdout */
                write(STDOUT_FILENO, buff, n);
            }
        }
    }
    return 0;
}
