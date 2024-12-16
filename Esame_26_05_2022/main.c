#include "prod_cons.h"
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/wait.h>

int main(){

    __pid_t pid;

    //creazione e inizializzazione strutture in memoria condivisa 
    int shm_id_buff_c=shmget(IPC_PRIVATE,sizeof(BufferCircolare),IPC_CREAT|0664);
    int shm_id_buff_s=shmget(IPC_PRIVATE,sizeof(BufferSingolo),IPC_CREAT|0664);

    BufferCircolare *bc=shmat(shm_id_buff_c,0,0);
    BufferSingolo *bs=shmat(shm_id_buff_s,0,0);

    for(int i=0;i<DIM;i++)
        bc->buffer[i]=0;

    bc->coda=bc->testa=bc->nelem=0;
    init_monitor(&(bc->m),2);

    bs->buffer=0;
    bs->pieno=0; //il buffer è vuoto
    init_monitor((&bs->m),1);

    //creazione processo produttore
    pid=fork();

    if(pid==0){
        produttore(bc,bs);
        exit(0);
    }else if(pid<0){
        perror("Errore fork");
    }

    //creazione processo consumatore
    pid=fork();

    if(pid==0){
        consumatore(bc,bs);
        exit(0);
    }else if(pid<0){
        perror("Errore fork");
    }

    for(int i=0;i<2;i++){
        wait(NULL);
    }

    //rimozione shared memories
    remove_monitor(&(bs->m));
    remove_monitor(&(bc->m));

    shmctl(shm_id_buff_c,IPC_RMID,NULL);
    shmctl(shm_id_buff_s,IPC_RMID,NULL);

    return 0;
}