#include "procedure.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <time.h>


float* leggi_risultati(struct MonitorRisultati * ls) {
	enter_monitor(&(ls->m));
	printf("[%d] Lettura - ingresso monitor\n", getpid());

	if(ls->numero_calcolatori>0){
		wait_condition(&(ls->m),VARCOND_PRINTERS);
	}
	ls->numero_printers++;
	signal_condition(&(ls->m),VARCOND_PRINTERS);

	leave_monitor(&(ls->m));
	printf("[%d] Lettura - uscita monitor\n", getpid());

	// var di appoggio in cui inserire i risultati letti dal monitor
	float* risultati = (float *)malloc(sizeof(float)*3);

	for(int i=0;i<N_OPS;i++){
		risultati[i]=ls->risultati[i];
	}

	enter_monitor(&(ls->m));
	printf("[%d] Lettura - ingresso monitor\n", getpid());

	ls->numero_printers--;
	if(ls->numero_printers==0){
		signal_condition(&(ls->m),VARCOND_CALCOLATORI);
	}
	leave_monitor(&(ls->m));
	printf("[%d] Lettura - uscita monitor\n", getpid());

	return risultati;
}

void inserisci_risultato(struct MonitorRisultati * ls, float valore, int operazione) {
	enter_monitor(&(ls->m));
	
	printf("[%d] Scrittura - ingresso monitor\n", getpid());

	if(ls->numero_printers>0 || ls->numero_calcolatori>0){
		printf("Scrittura - sospensione\n");
		wait_condition(&(ls->m),VARCOND_CALCOLATORI);
		printf("Scrittura - riattivazione\n");
	}
	ls->numero_calcolatori++;
	leave_monitor(&(ls->m));
	printf("[%d] Scrittura - uscita monitor\n", getpid());

	// la scrittura deve avvenire in ls->risultati in posizione 0 per la somma, 1 per il prodotto, 2 per la divisione
	ls->risultati[operazione-1]=valore;
	printf("[%d] Scrittura - valore [%f] operazione %d\n", getpid(),valore, operazione);

	enter_monitor(&(ls->m));
	printf("[%d] Scrittura - ingresso monitor\n", getpid());

	ls->numero_calcolatori--;

	if(queue_condition(&(ls->m),VARCOND_CALCOLATORI)>0){
		signal_condition(&(ls->m),VARCOND_CALCOLATORI);
	}else if(queue_condition(&(ls->m),VARCOND_PRINTERS)>0){
		signal_condition(&(ls->m),VARCOND_PRINTERS);
	}

	leave_monitor(&(ls->m));
	printf("[%d] Scrittura - uscita monitor\n", getpid());
}

void calcolo(struct MonitorRisultati * ls, int operazione, int mailbox){

	// riceve 2 operandi dalla coda per l'operazione indicata dal parametro "operazione" (ad es. 1=somma, 2=prodotto, 3=divisione)
	// effettua l'operazione indicata
	// inserisce il risultato nel monitor con la funzione inserisci_risultato
	// effettua tutte le operazioni di cui sopra 2 volte

	Messaggio m;
	float op[2],risultato;

	for(int i=0;i<2;i++){

		for(int j=0;j<2;j++){
			if(msgrcv(mailbox,(void *)&m,sizeof(Messaggio)-sizeof(long),operazione,0)==-1){
				perror("Errore receive");
			}else op[j]=m.valore;
		}

		sleep(1);
		
		if(operazione==1) risultato=op[0]+op[1];
		else if(operazione==2) risultato=op[0]*op[1];
		else risultato=op[0]/op[1]; //so per certo che op2 non è pari a 0

		inserisci_risultato(ls,risultato,operazione);
	}
}

void generazione(int operazione, int mailbox) {
	// crea un messaggio di ponendo il tipo uguale a "operazione" e il valore dell'operando casuale
	// invia il messaggio sulla coda
	// effettua tutte le operazioni di cui sopra 4 volte

	srand(time(NULL)*getpid());

	Messaggio m;
	m.tipo=operazione;

	for(int i=0;i<4;i++){
		m.valore=1 + (rand() / (float)RAND_MAX) * 19;
		printf("Il processo generatore %d ha generato un numero\n", getpid());
		if(msgsnd(mailbox,(void *)&m,sizeof(Messaggio)-sizeof(long),0)==-1){
			perror("Errore send");
		}
	}
}

void printer(struct MonitorRisultati * ls) {
	// già implementata, nulla da fare 
	float* risultati; 
	for (int i = 0 ; i < 2; i++) { //il gruppo di printer effettua N_PRINTERS*2 stampe, in questo caso (come da traccia) 6
		sleep(2);
		risultati = leggi_risultati(ls);
		printf("[%d] Risultati 1: %f, 2: %f, 3: %f\n",getpid(), risultati[0], risultati[1], risultati[2]);
		free(risultati);
	}
}
