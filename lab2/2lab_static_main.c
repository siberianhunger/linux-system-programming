#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

extern void SpaceExterminator();

int main(int argc, char *argv[]) {
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;

    if (argc != 4) {
        printf("Usage: %s old-file new-file exchange_number\n", argv[0]);
        exit(-1);
    }
/* Открытие файлов ввода и вывода */
    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        printf("Error opening file %s\n", argv[1]);
        exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        printf("Error opening file %s\n ", argv[2]);
        exit(-3);
    }

    SpaceExterminator(numRead, inputFd, outputFd, atoi(argv[3]));

    exit(EXIT_SUCCESS);
}