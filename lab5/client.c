#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define MSGSZ     128




int client(key_t key, int n, char massive[][30])
{
    int msqid;
    int msgflg = IPC_CREAT | 0666; 

    typedef struct msgbuf {
         long    mtype;
         char    mtext[MSGSZ];
         } message_buf;
    
    message_buf sbuf;
    size_t buf_length;

    printf("Calling msgget with key %#lx and flag %#o\n",key,msgflg);

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        exit(1);
    }
    else 
     printf("msgget: msgget succeeded: msqid = %d\n", msqid);


    sbuf.mtype = 1;
    printf("msgget: msgget succeeded: msqid = %d\n", msqid);
    
    // ---- отправка пачек
    for (int i = 0; i < n; i++)
    {
        memset(sbuf.mtext, 0, MSGSZ);
        (void) strcpy(sbuf.mtext, massive[i]);
     
        printf("msgget: msgget succeeded: msqid = %d\n", msqid);
        buf_length = strlen(sbuf.mtext) + 1;
        if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
            printf ("%d, %ld, %s, %ld\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
            perror("msgsnd");
            exit(1);
        }
        else 
            printf("Message: \"%s\" Send\n", sbuf.mtext);
    }
    return(0);
}