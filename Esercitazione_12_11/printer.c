#include "common.h"

void printer(int msgq_print)
{
	int counter = 1, i;
	//TODO definizione messaggio da ricevere
	msg_server m;

	printf("[printer] Printer Ready...\n");

	while(1)
	{
		//TODO ricezione messaggio contenente il buffer
		msgrcv(msgq_print,(void *)&m,sizeof(msg_server)-sizeof(long),0,0);
		
		for(i=0; i<BUFFER_DIM; i++)
		{
			// TODO se in posizione i-ma del buffer c'è un pid -1 il printer deve terminare
			if(m.list[i]==-1) exit(0);

			printf("{printer}\t[%u] %d\n", counter++, m.list[i]);
		}
	}
}
