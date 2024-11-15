#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#include "procedure.h"

int main() {

	int id_queue, id_shm;
	key_t chiave_shm = IPC_PRIVATE;
	key_t chiave_mbx = IPC_PRIVATE;

	id_shm = shmget(IPC_PRIVATE,sizeof(struct MonitorRisultati),IPC_CREAT|0664);
	id_queue = msgget(IPC_PRIVATE,IPC_CREAT|0664);

	if( id_shm < 0 || id_queue < 0) {
		perror("errore creazione risorse");
		exit(1);
	}

	struct MonitorRisultati * ls = shmat(id_shm,NULL,0);

	init_monitor(&(ls->m),2);
	ls->numero_calcolatori=0;
	ls->numero_printers=0;

	pid_t pid;

	int j;
	for(j=0; j < N_PRINTERS ; j++) {
		pid=fork();

		if(pid==0){
			printer(ls);
			exit(0);
		}else if(pid<0){
			perror("Errore fork");
			exit(1);
		}
	}
	
	for(j=0; j < N_CALCOLATORI ; j++) {
		//creazione processi calcolatori, uno per ogni operazione: 1 per la somma, 2 per il prodotto, 3 per la divisione
		pid=fork();

		if(pid==0){
			if(j==0) calcolo(ls,MSG_SOMMA,id_queue);
			else if (j==1) calcolo(ls,MSG_PRODOTTO,id_queue);
			else calcolo(ls,MSG_DIVISIONE,id_queue);

			exit(0);
		}else if(pid<0){
			perror("Errore fork");
			exit(1);
		}
	}

	for(j=0; j < N_GENERATORI ; j++) {
		// TODO creare processi generatori, uno per ogni operazione: 1 per la somma, 2 per il prodotto, 3 per la divisione
		pid=fork();

		if(pid==0){
			if(j==0) generazione(MSG_SOMMA,id_queue);
			else if (j==1) generazione(MSG_PRODOTTO,id_queue);
			else generazione(MSG_DIVISIONE,id_queue);
			exit(0);
		}else if(pid<0){
			perror("Errore fork");
			exit(1);
		}
	}

	for(j=0; j<N_CALCOLATORI+N_GENERATORI+N_PRINTERS; j++){
		pid=wait(NULL);
		printf("Il processo figlio %d è terminato\n",pid);
	}

	// TODO REMOVE RESOUCES
	msgctl(id_queue,IPC_RMID,0);
	remove_monitor(&(ls->m));
	shmctl(id_shm,IPC_RMID,0);
}





