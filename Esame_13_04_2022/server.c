#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include "header.h"

typedef struct {
    int value;
    
    // TODO completare con variabili per la sincronizzazione
    pthread_mutex_t mutex;
} data_str;

typedef struct {
    int queue_req;
    int queue_res;
    data_str* data;
} param;

void * update_worker(void* par) {
    
    //TODO prelievo parametro 'par'
    param *p=(param *)par;

    int i;
    for (i = 0; i < NUM_UPDATES; i++) {
        req msg;
        // TODO: ricezione del 'msg' di richiesta update
        msgrcv(p->queue_req,&msg,sizeof(req)-sizeof(long),REQ_UPDATER,0);

        int value=msg.value;
        printf("UPDATE WORKER: ricevuta richiesta, valore: %d\n",value);
        
        // TODO: aggiornamento del dato condiviso in mutua esclusione
        pthread_mutex_lock(&(p->data->mutex));

        p->data->value+=value;

        pthread_mutex_unlock(&(p->data->mutex));
    }
    pthread_exit(NULL);
}

void * reader_worker(void* par) {
    param* p = (param*) par;

    int i;
    for (i = 0; i < NUM_READS*NUM_READERS; i++) {
        req msg;
        // TODO: ricezione del 'msg' di richiesta lettura
        msgrcv(p->queue_req,&msg,sizeof(req)-sizeof(long),REQ_READER,0);

        int pid=msg.client_pid;
        printf("READER WORKER: ricevuta richiesta di lettura da: %d\n",pid);
        
        res risp;

        //TODO: preparazione e invio del messaggio di risposta
        pthread_mutex_lock(&(p->data->mutex));

        int value=p->data->value;

        pthread_mutex_unlock(&(p->data->mutex));

        risp.tipo=pid;
        risp.value=value;
        msgsnd(p->queue_res,&risp,sizeof(res)-sizeof(long),0);
    }
    pthread_exit(NULL);
}

int main() {

    key_t msg_req_key=ftok(".",'k');
	key_t msg_res_key=ftok(".",'j');

	int queue_req=msgget(msg_req_key,0);
	int queue_res=msgget(msg_res_key,0);

    pthread_t upd, rd;

    // TODO: allocare e inizializzare la struttura dati condivisa
    data_str *d=(data_str *)malloc(sizeof(data_str));
    d->value=0;
    pthread_mutex_init(&(d->mutex),NULL);

    param p;
    // TODO: preparare il parametro di scambio 'p'
    p.data=d;
    p.queue_req=queue_req;
    p.queue_res=queue_res;

    /* TODO: creare un tread update_worker e un thread
            reader_worker passando a entrambi 'p'*/
    pthread_create(&upd,NULL,update_worker,(void *)&p);
    pthread_create(&rd,NULL,reader_worker,(void *)&p);
    
    // TODO: completare con la join sui thread
    pthread_join(upd,NULL);
    pthread_join(rd,NULL);

    // TODO: completare con la rimozione della struttura dati e delle code
    pthread_mutex_destroy(&(d->mutex));
    free(d);

    return 0;
}