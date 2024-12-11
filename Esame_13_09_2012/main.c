#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main(){
    printf("[%d] - Main iniziato\n",getpid());

    key_t shm_key=ftok(".",'k');

    int shm_id=shmget(shm_key,sizeof(PriorityProdCons),IPC_CREAT|0664);

    PriorityProdCons *p=shmat(shm_id,0,0);

    inizializza_prod_cons(p);
    
    //istanzazione processi produttori H
    for(int i=0;i<NUM_PROD_H;i++){
        int pid=fork();

        if(pid==0){
            for(int i=0;i<3;i++){
                produci_alta_prio(p);
                sleep(2);
            }
            exit(0);
        }else if(pid<0)
            perror("Errore fork");
    }

    //istanzazione processi produttori L
    for(int i=0;i<NUM_PROD_L;i++){
        int pid=fork();

        if(pid==0){
            for(int i=0;i<3;i++){
                produci_bassa_prio(p);
                sleep(1);
            }
            exit(0);
        }else if(pid<0)
            perror("Errore fork");
    }

    //istanzazione processi consumatori
    for(int i=0;i<NUM_CONS;i++){
        int pid=fork();

        if(pid==0){
            for(int i=0;i<12;i++){
                consuma(p);
                sleep(1);
            }
            exit(0);
        }else if(pid<0)
            perror("Errore fork");
    }

    for(int i=0;i<NUM_CONS+NUM_PROD_H+NUM_PROD_L;i++){
        wait(NULL);
    }

    //rimozione strutture dati e monitor
    rimuovi_prod_cons(p);
    shmctl(shm_id,IPC_RMID,0);
}