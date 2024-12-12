#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include "header.h"


int main(){
	//ottenimento e attach shared memory
	key_t key_shm=ftok(".",'m');

	int ds_shm=shmget(key_shm,sizeof(struct Coda),0);

	struct Coda * c;
	c=shmat(ds_shm,NULL,0);
	
	//ottenimento semafori
	key_t key_sem=ftok(".",'k'); 

	int ds_sem=semget(key_sem,0,0);
    
	printf("[Client %d] - invio richieste...\n",getpid());

	int i=0;
	int num_prenotazione;	

	while(i<NUM_REQ){
		num_prenotazione=1+rand()%4; //numero casuale tra 1 e 5
		//invio richiesta tramite funzione "produci"
		produci(c,ds_sem,num_prenotazione);
	
		sleep(1);
		i++;
	}
	//TODO invio richiesta di terminazione (-1)
	printf("[Client %d] - Invio richiesta di terminazione\n",getpid());
	produci(c,ds_sem,-1);

	printf("[Client %d] - Fine processo\n",getpid());

	return 0;
}
