#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int pid;

    //TODO: completare l'inizializzazione code
    key_t key_queue_off=ftok(".",'k');
    key_t key_queue_risp=ftok(".",'j');
    int mq_offerte=msgget(key_queue_off,IPC_CREAT|0664);
    int mq_risposte=msgget(key_queue_risp,IPC_CREAT|0664);

    pid=fork();
    if (pid==0) {
        execl("./client_asta","./client_asta",NULL);
    }

    pid = fork();
    if (pid==0) {
        //TODO: exec del client_asta
        execl("./server_asta","./server_asta",NULL);
    }

    //TODO: attesa processi figli
    for(int i=0;i<2;i++){
        wait(NULL);
    }

    //TODO: eliminare le code
    msgctl(mq_offerte,IPC_RMID,0);
    msgctl(mq_risposte,IPC_RMID,0);

    return 0;
}
