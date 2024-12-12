#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "header.h"


int Wait_Sem (int id_sem, int numsem) {
			 int err;
			 struct sembuf sem_buf;

			 sem_buf.sem_num=numsem;
			 sem_buf.sem_flg=0;
			 sem_buf.sem_op=-1;

			 err = semop(id_sem,&sem_buf,1);   //semaforo rosso

			 if(err<0) {
				 perror("Errore WAIT");
			 }

			 return err;
}


int Signal_Sem (int id_sem, int numsem) {
			 int err;
			 struct sembuf sem_buf;

			 sem_buf.sem_num=numsem;
			 sem_buf.sem_flg=0;
			 sem_buf.sem_op=1;

			 err = semop(id_sem,&sem_buf,1);   //semaforo verde

			 if(err<0) {
				 perror("Errore SIGNAL");
			 }

			 return err;
}

void produci(struct Coda * c, int ds_sem, int num_prenotazioni){
	Wait_Sem(ds_sem,SPAZIO_DISP);

	Wait_Sem(ds_sem,MUTEXP);

	c->buffer[c->testa]=num_prenotazioni;
	c->testa=(c->testa+1)%DIM_BUFFER;
	printf("[CLIENT] - Valore Prodotto: %d\n",num_prenotazioni);

	Signal_Sem(ds_sem,MUTEXP);

	Signal_Sem(ds_sem,MSG_DISP);
}

int consuma(struct Coda * c, int ds_sem) {
		int consumato;

		Wait_Sem(ds_sem,MSG_DISP);

		Wait_Sem(ds_sem,MUTEXC);

		consumato=c->buffer[c->coda];
		c->coda=(c->coda+1)%DIM_BUFFER;
		printf("[WORKER] - Valore Consumato: %d\n",consumato);

	 	Signal_Sem(ds_sem,MUTEXC);

		Signal_Sem(ds_sem,SPAZIO_DISP);
		
		return consumato;
}

