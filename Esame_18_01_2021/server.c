#include "header.h"
#include <stdlib.h>

int queue_msg;

void *worker(void *);
void *collector(void *);

int main(){
    printf("[%d] - Server: Buongiorno\n",getpid());

    pthread_t th_w[NUM_WORKERS],th_c;
    struct param_worker *p[NUM_WORKERS];

    //recupero coda di messaggi
    __key_t msg_key=ftok(".",'k');

    queue_msg=msgget(msg_key,0);

    //allocazione e inizializzazione buffer condiviso
    struct buffer *b=(struct buffer *)malloc(sizeof(struct buffer));
    b->somma=0;
    b->num_update=0;
    pthread_cond_init(&(b->cv_prelievo),NULL);
    pthread_mutex_init(&(b->mutex),NULL);

    //creazione thread workers
    for(int i=0;i<NUM_WORKERS;i++){
        //creazione e inizializzazione struttura param_worker
        p[i]=(struct param_worker *)malloc(sizeof(struct param_worker));
        p[i]->buf=b;
        p[i]->id=i+1;

        pthread_create(&th_w[i],NULL,worker,(void *)p[i]); 
    }

    //creazione thread collector
    pthread_create(&th_c,NULL,collector,(void *)b); 

    //thread join
    for(int i=0;i<NUM_WORKERS;i++){
        pthread_join(th_w[i],NULL);
    }
    pthread_join(th_c,NULL);

    //deallocazione buffer condiviso e memoria 
    pthread_cond_destroy(&(b->cv_prelievo));
    pthread_mutex_destroy(&(b->mutex));
    free(b);
    
    //deallocazione strutture param_workers
    for(int i=0;i<NUM_WORKERS;i++){
        free(p[i]);
    }

    pthread_exit(0);
}

void *worker(void *p){

    struct param_worker *par=(struct param_worker *)p;  

    printf("[%d] - Thread worker %d: Buongiorno\n",getpid(),par->id);

    struct msg m;
    
    for(int i=0;i<NUM_MISURE;i++){
        msgrcv(queue_msg,&m,sizeof(struct msg)-sizeof(long),par->id,0);
        printf("[%d] - Thread worker %d: ricezione del messaggio avvenuta\n",getpid(),par->id);

        aggiorna(par->buf,m.measure);
    }

    pthread_exit(0);
}

void *collector(void *p){

    printf("[%d] - Thread collector: Buongiorno\n",getpid());

    struct buffer *b=(struct buffer *)p;

    int somma=preleva(b);

    double avg=somma/30;

    printf("[%d] - Collector: media=%f\n",getpid(),avg);

    pthread_exit(0);
}

void aggiorna(struct buffer *b, int value){
    
    pthread_mutex_lock(&(b->mutex));

    b->somma+=value;
    b->num_update++;

    printf("Num_update: %d\n",b->num_update);

    if(b->num_update==30)pthread_cond_signal(&(b->cv_prelievo));

    pthread_mutex_unlock(&(b->mutex));

}

int preleva(struct buffer *b){
    pthread_mutex_lock(&(b->mutex));

    while(b->num_update!=30){
        pthread_cond_wait(&(b->cv_prelievo),&(b->mutex));
    }
    
    int value=b->somma;

    pthread_mutex_unlock(&(b->mutex));

    return value;
}


