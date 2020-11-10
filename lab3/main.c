// Многозадачное программирование в Linux
// Программа из учебника "Системное программирование в среде Linux", Гунько А.В., стр. 63
// Название: spaces
// Описание: родительская программа
// Входные параметры: список имен файлов для обработки
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int i, pid[argc], status, stat;
    char arg[20];
// для всех файлов, перечисленных в командной строке
    if (argc<2) {
        printf("Usage: programm spaces_to_delete textfile1 textfile2  ...\n");
        exit(-1);
    }
    for (i = 1; i< argc; i++) {
// запускаем дочерний процесс
        strcpy(arg,argv[i]);
        pid[i] = fork();
        if (pid[i] == 0) {
// если выполняется дочерний процесс
// вызов функции счета количества пробелов в файле

            if (execl("./dynamic.bin","dynamic.bin",argv[i], argv[i + 1], argv[1][0], NULL)<0) {
                printf("ERROR while start processing file %s\n",argv[i]);
                exit(-2);
            }
            else printf( "processing of file %s started (pid=%d)\n", argv[i],pid[i]);
        }
// если выполняется родительский процесс
    }
    sleep(1);
// ожидание окончания выполнения всех запущенных процессов
    for (i = 1; i< argc; i++) {
        status=waitpid(pid[i],&stat,0);
        if (pid[i] == status) {
            printf("File %s done,  result=%d\n",argv[i],WEXITSTATUS(stat));
        }
    }
    return 0; }
