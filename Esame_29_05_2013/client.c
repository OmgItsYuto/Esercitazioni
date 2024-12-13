#include "header.h"
#include <stdlib.h>
#include <sys/msg.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    printf("[%d]- Client: Buongiorno\n",getpid());

    srand(time(NULL)*getpid());

    __key_t req_msg_key=ftok(".",'k');
    __key_t res_msg_key=ftok(".",'j');

    //recupero code
    int queue_req=msgget(req_msg_key,0);
    int queue_res=msgget(res_msg_key,0);

    for(int i=0;i<5;i++){
        struct msg_req m;
        m.tipo=1;
        m.client_pid=getpid();
        for(int i=0;i<2;i++)
            m.values[i]=rand()%10;

        printf("[%d]- Client: Invio %d°/5 messaggio -> {%d, %d}\n",getpid(),i+1,m.values[0],m.values[1]);

        msgsnd(queue_req,&m,sizeof(struct msg_req)-sizeof(long),0);

        struct msg_res res;
        msgrcv(queue_res,&res,sizeof(struct msg_res)-sizeof(long),getpid(),0);
        printf("[%d]- Client: Prodotto -> %d\n",getpid(),res.product);
    }

    return 0;
}