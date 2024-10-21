#include <stdio.h>
#include <sys/shm.h>

int main(){
    int key=ftok(".",'k');
    int ds_shm=shmget(key,0,0); //sappiamo che la risorsa condivisa esiste già

    char *s=(char *) shmat(ds_shm,NULL,0);

    printf("Contenuto SHM: %s",s);
}