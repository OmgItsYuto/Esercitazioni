#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include "header.h"


int main() {
    printf("[%d] - Client iniziato\n",getpid());	

	int pid;

	// completare l'inizializzazione code
    key_t msg_req_key = ftok(".",'m');
	key_t msg_res_key = ftok(".",'k');

	int queue_req = msgget(msg_req_key,0);
	int queue_res = msgget(msg_res_key,0);

	int i;
	for (i=0; i < NUM_CONS; i++) {
		req msg;
		//  preparazione messaggio
		msg.type=1;  //  modificare
		printf("[%d] - CONSUMATORE: invio richiesta \n",getpid());
		//  invio richiesta
		msgsnd(queue_req,&msg,sizeof(req)-sizeof(long),0);

		res risp;
		//  ricezione risposta
		msgrcv(queue_res,&risp,sizeof(res)-sizeof(long),0,0);

		printf("[%d] - CONSUMATORE: letto valore: %d\n",getpid(),risp.value);
		sleep(1);
	}

	return 0;
}
