#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#define BUF_SIZE 1024

int SpaceExterminator(ssize_t numRead, int inputFd, int outputFd, int spaces_to_delete) {
    int count = 0;
    int exchange = 0;
    char buf[BUF_SIZE] = {0};
    int result = 0;
// Посимвольно читаем файл
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        printf("%s", buf);

        while (count < BUF_SIZE) {
// Считаем количество удалённых пробелов(exchange) и сравниваем с заданным на вход кол-ом требуемых удалений(argv[3])
            if (exchange < spaces_to_delete) {
                if (buf[count] != 0x20) {
                    printf("%c", buf[count]);
                    write(outputFd, &buf[count], 1);
                } else { exchange++; }

            } else {
                printf("%c", buf[count]);
                write(outputFd, &buf[count], 1);
            }
            count++;
        }
        result = 1;
        if (numRead == -1) {
            printf("read error\n ");
            result = 0;
            exit(-5);
        }
        if (close(inputFd) == -1) {
            printf("close input error\n");
            result = 0;
            exit(-6);
        }
        if (close(outputFd) == -1) {
            printf("close output error\n");
            result = 0;
            exit(-7);
        }
    }
    return result;
}

