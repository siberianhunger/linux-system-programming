#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#define MAXLINE 90
#define BUF_SIZE 1024

void pipe_server(int readfd, int writefd) {
    ssize_t n;
    char args[MAXLINE + 1];
    char buff[BUF_SIZE];

    /* получение строки с тройкой аргументов из канала IPC */
    if ((n = read(readfd, args, MAXLINE)) == 0) {
        printf("end-of-file while reading pathname");
        exit(1);
    }
    args[n] = '\0';	/* завершается 0 */

    /* разбиение строки на три аргумента */
    char *argv[3];
    char *tok;
    int i = 1;
    tok = strtok(args," ");
    while (tok != NULL)  {
        argv[i] = tok;
        i = i + 1;
        tok = strtok(NULL," ");
    }
    int inputFd, outputFd, openFlags, result;
    mode_t filePerms ;
    ssize_t numRead;
    void *ext_library;	// хандлер внешней библиотеки
    i = 0;
    /* Открытие файлов ввода и вывода */
    inputFd = open (argv[2], O_RDONLY);
    if (inputFd == -1) {
        printf ("Error opening file %s\n", argv[1]) ; exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    outputFd = open (argv[3], openFlags, filePerms);
    if (outputFd == -1) {
        printf ("Error opening file %s\n ", argv[3]) ; exit(-3);
    }

    ext_library = dlopen("./libpowers.so", RTLD_LAZY);
    if (!ext_library){
        //если ошибка, то вывести ее на экран
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        return;
    }
    printf("Library loaded!\n");
    int (*SpaceExterminator)(ssize_t, int, int, int);
    SpaceExterminator = dlsym(ext_library, "SpaceExterminator");
    result = SpaceExterminator(numRead,  atoi(argv[1]), inputFd, outputFd);
    dlclose(ext_library);

    sprintf(buff, "File %s done, result=%d\n", argv[3], result);
    write(writefd, buff, strlen(buff));
}
