#include "common.h"
#include <stdio.h>

void server(int msgq_guest, int msgq_print)
{
	int i;
	
	/* TODO Definizione Messaggio da ricevere dal proc. client */
	msg_client m1;

	/* TODO Definizione Messaggio da inviare al proc. printer */
	msg_server m2;
	m2.tipo=SERVER_PRINTER_MESSAGE;

	printf("[server ] Server ready...\n");
	while(1)
	{
		for(i=0; i<BUFFER_DIM; i++)
		{
			/*TODO Ricezione del messaggio di richiesta*/
			msgrcv(msgq_guest,(void *)&m1,sizeof(msg_client)-sizeof(long),0,0);
			printf("Il server %d riceve il messaggio %d\n",getpid(),m1.processo);
			
			/*TODO se il messaggio è di tipo fine, inserisce -1 in posizione i-ma e termina*/
			if(m1.tipo==END_MESSAGE){
				m1.processo=-1;
				m2.list[i]=m1.processo;
				msgsnd(msgq_print,&m2,sizeof(msg_server)-sizeof(long),0);
				exit(0);
			}else{
				m2.list[i]=m1.processo;
			}
		}
		
	/*TODO Invio del messaggio con il buffer completo al proc. printer */
	msgsnd(msgq_print,(void *)&m2,sizeof(msg_server)-sizeof(long),0);
	printf("Messaggio per il printer\n");
	}
}
