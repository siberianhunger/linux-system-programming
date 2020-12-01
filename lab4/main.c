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

    if (argc < 4 || (argc - 1) % 3 != 0) {
        printf("Usage: spaces_to_delete inp_file1 out_file1 spaces_to_delete inp_file2 out_file2 ...\n");
        exit(-1);
    }

    int countInput = (argc - 1) / 3;
    int i, pid[countInput];
    int pipe1[countInput][2], pipe2[countInput][2];
    char buff[MAXLINE];
    char arg1[30], arg2[30], arg3[30];

    for (i = 0; i < countInput; i += 1) {
        pipe(pipe1[i]);
        pipe(pipe2[i]);

        pid[i] = fork();
        if (pid[i] == 0) {
            /* child */
            close(pipe1[i][1]);
            close(pipe2[i][0]);
            pipe_server(pipe1[i][0], pipe2[i][1]);
            exit(0);
        }
    }

    for (i = 0; i < countInput; i += 1) {
        strcpy(arg1, argv[i * 3 + 1]);
        strcpy(arg2, argv[i * 3 + 2]);
        strcpy(arg3, argv[i * 3 + 3]);
        sprintf(buff, "%s %s %s", arg1, arg2, arg3);


        write(pipe1[i][1], buff, strlen(buff));
    }

    sleep(1);

    int n;
    for (i = 0; i < countInput; i += 1) {
        memset(buff, 0, MAXLINE);
        if (pid[i] == waitpid(pid[i], NULL, 0)) {
            if ((n = read(pipe2[i][0], buff, MAXLINE)) > 0) {
                write(STDOUT_FILENO, buff, n);
            }
        }
    }
    return 0;
}
