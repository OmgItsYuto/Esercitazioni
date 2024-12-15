#include "header.h"
#include <stdlib.h>

int main(int argc, char *argv[]){
    srand(time(NULL)*getpid());

    int id_client = atoi(argv[1]);

    printf("[%d] - Client %d: Buongiorno\n",getpid(),id_client);

    //recupero coda di messaggi
    __key_t msg_key=ftok(".",'k');

    int queue_msg=msgget(msg_key,0);

    for(int i=0;i<10;i++){
        struct msg m;
        m.id_client=id_client;
        m.tipo=m.id_client;
        m.measure=rand()%100;

        printf("[%d] - Client %d: Invio del messaggio, measure -> %d\n",getpid(),id_client,m.measure);

        msgsnd(queue_msg,&m,sizeof(struct msg)-sizeof(long),0);

        sleep(2);
    }

    return 0;
}