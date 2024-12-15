#include "header.h"
#include <sys/ipc.h>
#include <sys/wait.h>

int main(){
    printf("[%d] - Main: Buongiorno\n",getpid());

    pid_t pid;

    char id_client_str[10];

    //creazione coda di messaggi
    __key_t msg_key=ftok(".",'k');

    int queue_msg=msgget(msg_key,IPC_CREAT|0664);

    //creazione processo server
    pid=fork();

    if(pid==0){
        execl("./server","./server",NULL);
        _exit(1);
    }else if(pid<0){
        perror("Errore fork");
    }

    //creazione processi clients
    for(int i=0;i<NUM_CLIENTS;i++){
        pid=fork();

        if(pid==0){
            snprintf(id_client_str, sizeof(id_client_str), "%d", i+1);
            execl("./client","./client",id_client_str,NULL);
            _exit(1);
        }else if(pid<0){
            perror("Errore fork");
        }
    }

    //join dei processi
    for(int i=0;i<NUM_CLIENTS+1;i++){
        wait(NULL);
    }

    //rimozione coda
    msgctl(queue_msg,IPC_RMID,0);

    return 0;
}