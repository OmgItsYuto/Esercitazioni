#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include "header.h"
#include <time.h>
#define MAX_OFF 10

void client(int my_id, int mq_off, int mq_res) {
    int base_asta = 0;
    int aggiudicata = 0;

    while(!aggiudicata) {
        int offerta = base_asta + rand()%MAX_OFF+1;
        offerta_msg off;
        
        //compilazione messaggio 'off' e invio al server
        off.id_cliente=my_id;
        off.valore_offerto=offerta;

        msgsnd(mq_off,&off,sizeof(offerta_msg)-sizeof(long),0);

        printf("CLIENT %d: invio offerta %d\n",my_id,offerta);
        
        risposta_offerta_msg risp;
        //ricezione messaggio 'risp' per my_id con stato offerta corrente 
        msgrcv(mq_res,&risp,sizeof(risposta_offerta_msg)-sizeof(long),my_id,0);
        
        aggiudicata=risp.asta_aggiudicata;

        if(aggiudicata) {
            printf("CLIENT %d: asta aggiudicata da ",my_id);
            if (risp.id_vincitore==my_id) {
                printf("ME, SONO IL VINCITORE!!\n");
            } else printf("%d\n",risp.id_vincitore);
        } else {
            //rialzo base d'asta in base al valore corrente
            base_asta = risp.max_valore_offerto;
            printf("CLIENT %d: max valore offerta %d, rialzo!\n",my_id,risp.max_valore_offerto);
        }
        sleep(1);
    }
}

int main() {

    int pid;

    //inizializzazione code
    key_t key_queue_off=ftok(".",'k');
    key_t key_queue_risp=ftok(".",'j');
    int mq_offerte=msgget(key_queue_off,0);
    int mq_risposte=msgget(key_queue_risp,0);

    for (int i=1;i<MAX_CLIENTI+1;i++) {
        // TODO: genera i clienti con fork, passando 'i' come my_id
        pid=fork();

        if(pid==0){
            srand(time(NULL)*getpid());
            client(i,mq_offerte,mq_risposte);
            exit(0);
        }else if(pid<0){
            perror("Errore fork");
        }
    }

    for (int i=1;i<MAX_CLIENTI+1;i++){ 
        wait(NULL);
    }

    return 0;
}
