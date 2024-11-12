#include <sys/wait.h>
#include "common.h"


#define NR_CLIENT  5

/* ID - Code di Messaggi */
int msgq_guest;
int msgq_print;

int main(int argc,char*argv[])
{
	int i;
	pid_t pid;
	/*Richiesta code di messaggi IPC*/
	msgq_guest = msgget(IPC_PRIVATE,IPC_CREAT|0664);
	msgq_print = msgget(IPC_PRIVATE,IPC_CREAT|0664); 

	/*Creazione processo 'printer'*/
	pid=fork();

	if(pid==0){
		printer(msgq_print);
		exit(0);
	}else if(pid==-1){
		perror("Errore fork");
		exit(-1);
	}

	/*Creazione processo 'server'*/
	pid=fork();

	if(pid==0){
		server(msgq_guest,msgq_print);
		exit(0);
	}else if(pid==-1){
		perror("Errore fork");
		exit(-1);
	}

	/*Creazione processi 'client'*/
	for(i=0;i<NR_CLIENT;i++){
		pid=fork();

		if(pid==0){
			client(msgq_guest);
			exit(0);
		}else if(pid==-1){
			perror("Errore fork");
			exit(-1);
		}
	}

	/*Attesa terminazione dei client*/
	int j=0;
	while (j<NR_CLIENT)
	{
		if((pid=wait(NULL))==-1)perror("Errore wait");
		printf("Il client %d è terminato\n",pid);
		j++;
	}

	msg_client m;
	m.tipo=END_MESSAGE;


	/*Invio messaggio di terminazione al server*/
	if(msgsnd(msgq_guest,(void *)&m,sizeof(msg_client)-sizeof(long),0)==-1)perror("Errore send");
	printf("Messaggio inviato al server per la richiesta di fine\n");

	/*Attesa terminazione processi 'server' e 'printer'*/
	for(i=0; i<2; i++){
		pid=wait(0);
        printf("Il processo %d è terminato\n",pid);
    }

	/*Rimozione code IPC e uscita*/	
	msgctl(msgq_guest,IPC_RMID,0);
	msgctl(msgq_print,IPC_RMID,0);

	return 0;
}
