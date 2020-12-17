#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MSGSZ     128




int client(key_t key, int n, char massive[][30])
{
    int semid, shmid;
    struct sembuf sops[2];
    char *shmaddr;
    char str[MSGSZ];
    if ((shmid = shmget(key, MSGSZ, IPC_CREAT | 0666)) < 0) { perror("shmget"); return 1; }
    if ((shmaddr = (char*)shmat(shmid, NULL, 0)) == (void*)-1) { perror("shmat"); return 1; }

    semid = semget(key, 2, IPC_CREAT | 0666);
    semctl(semid, 0, IPC_SET, 0);
    sops[0].sem_num = 0;
    sops[0].sem_flg = 0;
    sops[1].sem_num = 0;
    sops[1].sem_flg = 0;

    //ждем готовность сервера
    printf("Client> Wait start Server\n");
    sops[1].sem_op = -1;
    semop(semid, &sops[1], 1);
    // ---- отправка пачек
    for (int i = 0; i < n; i++){
        //sleep(0.2);
        memset(shmaddr, 0, MSGSZ);
        strcpy(shmaddr, massive[i]);
        printf("CLIENT> packege %d copied to the shared buffer\n", i);
        // Освобождаем доступ
        sops[0].sem_op = 1; 
        semop(semid, &sops[0], 1);
        printf("CLIENT> Access released to the shared buffer\n");
        //ждем готовность сервера
        sops[1].sem_op = -1;
        semop(semid, &sops[1], 1);
    }
    shmdt(shmaddr); 
    semctl(semid,0,IPC_RMID, 0); 
    shmctl(shmid,IPC_RMID,NULL); 
    return(0);
}