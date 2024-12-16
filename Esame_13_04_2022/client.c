#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "header.h"

void updater(int queue_req) {
	srand(time(NULL)*getpid());
	
	int i;
	for (i=0;i<NUM_UPDATES;i++) {
		int val = rand()%100 - 40;
		req msg;
		/*TODO completare con preparazione e invio messaggio di 
		richiesta aggiornamento 'msg' con il valore 'val' */
		msg.client_pid=0;
		msg.tipo=REQ_UPDATER;
		msg.value=val;

		msgsnd(queue_req,&msg,sizeof(req)-sizeof(long),0);

		printf("UPDATER: invio richiesta aggiornamento a valore %d\n",val);
	
		sleep(2);
	}
}

void reader(int queue_req, int queue_res) {
	int i;
	for (i=0; i < NUM_READS; i++) {
		req msg;
		/*TODO completare con preparazione e invio messaggio di 
		richiesta di lettura 'msg' */
		msg.client_pid=getpid();
		msg.tipo=REQ_READER;
		msg.value=0;

		msgsnd(queue_req,&msg,sizeof(req)-sizeof(long),0);
		printf("READER %d: invio richiesta lettura\n",getpid());

		/*TODO completare con ricezione risposta */
		res msg_res;
		
		msgrcv(queue_res,&msg_res,sizeof(res)-sizeof(long),getpid(),0);

		int value=msg_res.value;
		printf("READER %d: letto valore: %d\n",getpid(),value);
		sleep(1);
	}
}

int main() {
	int pid;
	key_t msg_req_key=ftok(".",'k');
	key_t msg_res_key=ftok(".",'j');

	int queue_req=msgget(msg_req_key,0);
	int queue_res=msgget(msg_res_key,0);

	srand(time(NULL));
	
	// TODO creare un processo figlio che esegue la funzione 'updater'
	pid=fork();

	if(pid==0){
		updater(queue_req);
		exit(0);
	}else if(pid<0){
		perror("Errore fork");
	}

	// TODO creare NUM_READERS processi figlio che eseguono la funzione 'reader'
	for (int i=0;i<NUM_READERS;i++) {
		// TODO creazione del processo reader
		pid=fork();
		
		if(pid==0){
			reader(queue_req,queue_res);
			exit(0);
		}else if(pid<0){
			perror("Errore fork");
		}
	}

	// TODO mettere in attesa il padre dei processi figli
	for(int i=0;i<NUM_READERS+1;i++)
		wait(NULL);
	
	// TODO completare con rimozione delle code -> ma che cazz

	return 0;
}