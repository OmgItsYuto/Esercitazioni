#include "common.h"

void client(int msgq_guest)
{
	printf("Client %d\n",getpid());
	int i;

	/* Inizializzazione Messaggio del processo */
	msg_client m;
	m.tipo=CLIENT_SERVER_MESSAGE;
	m.processo=getpid();

	i=0;
	/* Invio dei 15 messaggi in ciclo, con sleep(1) tra un messaggio e l'altro*/
	while(i<15){
		if(msgsnd(msgq_guest,(void *)&m,sizeof(msg_client)-sizeof(long),0)==-1){
			perror("Errore send");
		}else{
			printf("Il client %d manda il messaggio num %d\n",getpid(),i+1);
		}

		sleep(1);
		i++;	
	}
}
