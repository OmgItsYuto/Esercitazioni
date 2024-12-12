#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>
#include "header.h"

#define NUM_WORKER 3

static struct Coda * coda;
static int ds_sem;
static pthread_t threads[NUM_WORKER];
static int end=0;

// struttura per passaggio parametri ai thread
typedef struct {
	int id;
	Count* count;
} parametri;

void init_buffer(Count* c){
	//inizializzazione Count
	c->num_prenotazioni=0;
	pthread_mutex_init(&(c->mutex),NULL);
}

void remove_buffer(Count* c){
	//rimozione Count
	pthread_mutex_destroy(&(c->mutex));
}

void *worker(void* arg){

	printf("[WORKER] - start del thread...\n");

	parametri* par=(parametri *)arg;
	
	while (!end) {
		//TODO completare il codice del worker
		// worker preleva dalla coda con funzione "consuma"
		// aggiornamento (eventuale) variabile end se valore ricevuto = -1
		// aggiornamento Count in mutua esclusione	
		int value=consuma(coda,ds_sem);
		if(value==-1){
			end=1;
		}else{
			pthread_mutex_lock(&(par->count->mutex));

			par->count->num_prenotazioni++;

			pthread_mutex_unlock(&(par->count->mutex));
		}
	}
    //uscita dal thread
	pthread_exit(0);
}

int main(){

	int i,k;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

	//ottenimento e attach shared memory
	key_t key_shm=ftok(".",'m');

	int ds_shm=shmget(key_shm,sizeof(struct Coda),0);

	coda=shmat(ds_shm,NULL,0);
	
	//ottenimento semafori
	key_t key_sem=ftok(".",'k'); 

	ds_sem=semget(key_sem,0,0);

	//TODO Creazione e Inizializzazione struttura locale
	Count* count=(Count *)malloc(sizeof(Count));
	init_buffer(count);
	
	parametri param[NUM_WORKER];

	//Avvio dei thread
	for(i=0;i<NUM_WORKER;i++){
		printf("[SERVER] - creo thread %d...\n",i);
		//creazione threads
		param[i].id=i;
		param[i].count=count;
		pthread_create(&threads[i],&attr,worker,(void *)&param[i]);
	}
	
	for(k=0;k<NUM_WORKER;k++) {
		//join
	  	pthread_join(threads[k],NULL); 
	}
	
	printf("Numero prenotazioni consumate: %d\n",count->num_prenotazioni);

	//rimozione struttura locale
	remove_buffer(count);
	free(count);

	printf("[SERVER] - bye bye...\n");
	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
}

