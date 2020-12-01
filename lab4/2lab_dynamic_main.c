#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <dlfcn.h>
#define BUF_SIZE 1024
extern int SpaceExterminator();

int main(int argc, char *argv[]) {
    int inputFd, outputFd, openFlags, result = -1;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE] = {0};
    void *ext_library;	// хандлер внешней библиотеки
    if (argc != 4) {
        printf("Usage: %s spaces_to_delete old-file new-file\n", argv[0]);
        exit(-1);

    }
/* Открытие файлов ввода и вывода */
    inputFd = open(argv[2], O_RDONLY);
    if (inputFd == -1) {
        printf("Error opening file %s\n", argv[1]);
        exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    outputFd = open(argv[3], openFlags, filePerms);
    if (outputFd == -1) {
        printf("Error opening file %s\n ", argv[2]);
        exit(-3);
    }
    ext_library = dlopen("./libpowers.so",RTLD_LAZY);
    if (!ext_library){
        //если ошибка, то вывести ее на экран
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        return 1;
    };

    int (*SpaceExterminator)(ssize_t, int, int, int);
    SpaceExterminator = dlsym(ext_library, "SpaceExterminator");
    result = SpaceExterminator(numRead,  atoi(argv[1]), inputFd, outputFd);
    return result;
}
