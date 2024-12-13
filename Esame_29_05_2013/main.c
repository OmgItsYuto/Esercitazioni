#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "header.h"
#include <sys/wait.h>

int main(){
    printf("[%d]- Main: Buongiorno\n",getpid());

    pid_t pid,pidc[NUM_CLIENTS];

    __key_t req_msg_key=ftok(".",'k');
    __key_t res_msg_key=ftok(".",'j');

    //creazione code
    int queue_req=msgget(req_msg_key,IPC_CREAT|0664);
    int queue_res=msgget(res_msg_key,IPC_CREAT|0664);

    pid=fork();

    if(pid==0){
        execl("./server","./server",NULL);
        exit(1);
    }else if(pid<0){
        perror("Errore fork");
        exit(1);
    }

    for(int i=0;i<NUM_CLIENTS;i++){
        pid=fork();

        if(pid==0){
            pidc[i]=getpid();
            execl("./client","./client",NULL);
            exit(1);
        }else if(pid<0){
            perror("Errore fork");
            exit(1);
        }
    }

    for(int i=0;i<NUM_CLIENTS;i++){
        waitpid(pidc[i],NULL,0);
    }
    
    sleep(3);

    //invio messaggio al server
    struct msg_req m;
    m.tipo=1;
    m.client_pid=getpid();
    m.values[0]=m.values[1]=-1;

    printf("[%d]- Main: Invio messaggio di terminazione\n",getpid());
    msgsnd(queue_req,&m,sizeof(struct msg_req)-sizeof(long),0);

    wait(NULL);

    //rimozione code
    msgctl(queue_req,IPC_RMID,0);
    msgctl(queue_res,IPC_RMID,0);

    return 0;
}