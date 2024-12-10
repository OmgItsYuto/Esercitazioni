#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"
#include <pthread.h>

typedef struct {
    int buffer[MAX_VALUES];

    // aggiungere le variabili per la gestione circolare
    // e per la sincronizzazione dei thread
    int testa;
    int coda;
    int cnt;

    pthread_cond_t cond_produttore;
    pthread_cond_t cond_consumatore;
    pthread_mutex_t mutex_buf;
    
} data_str;

typedef struct {
    int somma;
    int num_valori_sommati;

    // aggiungere le variabili per la sincronizzazione dei thread
    pthread_cond_t cond_stampa;
    pthread_mutex_t mutex_sum;

} somma_valori;

//struttura per passare i parametri somma_valori e data_str ai thread
typedef struct {
    somma_valori* sum;
    data_str* buf;
} parametri;


// le code
int queue_req;
int queue_res;

int consuma(data_str* buf) {
    int value;

    // consumatore, con gestione coda circolare
    pthread_mutex_lock(&(buf->mutex_buf));

    while(buf->cnt==0){
        pthread_cond_wait(&(buf->cond_consumatore),&(buf->mutex_buf));
    }

    value=buf->buffer[buf->testa];
    buf->testa=(buf->testa+1)%MAX_VALUES;
    buf->cnt--;

    pthread_cond_signal(&(buf->cond_produttore));

    pthread_mutex_unlock(&(buf->mutex_buf));

    return value;
}

void produci(data_str* buf, int new_value) {
    // produttore, con gestione coda circolare
    pthread_mutex_lock(&(buf->mutex_buf));

    while(buf->cnt==MAX_VALUES){
        pthread_cond_wait(&(buf->cond_produttore),&(buf->mutex_buf));
    }

    buf->buffer[buf->coda]=new_value;
    buf->coda=(buf->coda+1)%MAX_VALUES;
    buf->cnt++;

    pthread_cond_signal(&(buf->cond_consumatore));

    pthread_mutex_unlock(&(buf->mutex_buf));
}


void * stampa_somma(void *par) {
    printf("[%d] - Thread stampa iniziato\n",getpid());

    somma_valori * s = ((parametri *)par)->sum;
    
    // attesa che siano stati sommati NUM_CONS valori utilizzando una condition
    // e stampa della somma una volta verificata la condizione
    pthread_mutex_lock(&(s->mutex_sum));

    while(!(s->num_valori_sommati==NUM_CONS)){
        pthread_cond_wait(&(s->cond_stampa),&(s->mutex_sum));
    }

    printf("Somma: %d\n", s->somma);

    pthread_mutex_unlock(&(s->mutex_sum));

    pthread_exit(NULL);
}

void * produttore(void *par) {
    printf("[%d] - Thread produttore iniziato\n",getpid());

    data_str * buf = ((parametri *)par)->buf;
    int new_value;
    int i;
    for (i = 0; i < NUM_UPDATES*MAX_VALUES; i++) {
        new_value = rand()%10+1;
        printf("[%d] - PRODUTTORE: inserimento nuovo dato: %d\n",getpid(),new_value);
        produci(buf,new_value);
        sleep(rand()%3+1);
    }
    pthread_exit(NULL);
}


void * consumatore(void *par) {
    printf("[%d] - Thread consumatore iniziato\n",getpid());

    parametri * p = (parametri *)par;
    data_str* buf = p->buf;
    somma_valori* sum = p->sum;
    int i;
    for (i = 0; i < NUM_CONS; i++) {
        req msg;
        // ricezione messaggio di richiesta
        msgrcv(queue_req,&msg,sizeof(req)-sizeof(long),0,0);
        printf("[%d] - CONSUMATORE_SERV: ricevuta richiesta di consumo\n",getpid());

        res risp;
        // preparazione messaggio di risposta
        //prelevando il valore dalla funzione "consuma"
        risp.type = 1;
        risp.value = consuma(buf);

        printf("[%d] - CONSUMATORE_SERV: invio valore al consumatore client\n",getpid());
        // invio risposta
        msgsnd(queue_res,&risp,sizeof(res)-sizeof(long),0);;
        
        // aggiornamento in mutua esclusione della somma in 'sum' con il nuovo valore appena consumato
        // e notifica della condizione quando sommati NUM_CONS elementi
        pthread_mutex_lock(&(sum->mutex_sum));

        sum->somma+=risp.value;
        sum->num_valori_sommati++;
        if(sum->num_valori_sommati==NUM_CONS) pthread_cond_signal(&(sum->cond_stampa));

        pthread_mutex_unlock(&(sum->mutex_sum));
    }

    pthread_exit(NULL);
}

int main() {
    printf("[%d] - Server iniziato\n",getpid());

    // completare l'inizializzazione code
    key_t msg_req_key = ftok(".",'m');
	key_t msg_res_key = ftok(".",'k');

    queue_req = msgget(msg_req_key,0);
	queue_res = msgget(msg_res_key,0);

    pthread_t prod, cons, sum;

    srand(time(NULL));

    somma_valori* s = (somma_valori *)malloc(sizeof(somma_valori));
    // inizializzazione campi di s
    s->num_valori_sommati=0;
    s->somma=0;
    pthread_cond_init(&(s->cond_stampa),NULL);
    pthread_mutex_init(&(s->mutex_sum),NULL);

    data_str* shared_buf = (data_str *)malloc(sizeof(data_str));
    // inizializzazione campi di shared_buf
    shared_buf->coda=shared_buf->testa=shared_buf->cnt=0;
    
    for(int i=0;i<MAX_VALUES;i++)
        shared_buf->buffer[i]=0;

    pthread_cond_init(&(shared_buf->cond_consumatore),NULL);
    pthread_cond_init(&(shared_buf->cond_produttore),NULL);
    pthread_mutex_init(&(shared_buf->mutex_buf),NULL);

    parametri * par = (parametri *)malloc(sizeof(parametri));
    // inizializzazione campi di par
    par->buf=shared_buf;
    par->sum=s;
    
    // creazione dei tre thread prod cons e sum
    /* nota: al thread produttore basta passare solo il buffer 'shared_buf'
             al thread stampa basta passare solo la somma 's'
             al thread consumatore bisogna passare entrambi tramite 'par'
    */
    pthread_create(&prod,NULL,produttore,(void *)par);
    pthread_create(&sum,NULL,stampa_somma,(void *)par);
    pthread_create(&cons,NULL,consumatore,(void *)par);

    // join 
    pthread_join(prod,NULL);
    pthread_join(sum,NULL);
    pthread_join(cons,NULL);

    // rimozione strutture dati
    pthread_mutex_destroy(&(s->mutex_sum));
	pthread_cond_destroy(&(s->cond_stampa));
    free(s);

    pthread_mutex_destroy(&(shared_buf->mutex_buf));
	pthread_cond_destroy(&(shared_buf->cond_consumatore));    
  	pthread_cond_destroy(&(shared_buf->cond_produttore));    
    free(shared_buf);

    free(par);

    pthread_exit(0);    
}