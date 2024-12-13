#include "header.h"
#include <stdlib.h>
#include <sys/msg.h>
#include <pthread.h>
#include <unistd.h>

int queue_res,queue_req;

void *worker(void *p){
    printf("[%d]- Thread worker: Buongiorno\n",getpid());

    struct buf_circolare *b=(struct buf_circolare *)p;

    while(1){
        pthread_mutex_lock(&(b->mutex));

        while(b->nelem==0)
            pthread_cond_wait(&(b->cv_cons),&(b->mutex));

        struct msg_req m=b->buf[b->coda];
        b->coda=(b->coda+1)%DIM;
        b->nelem--;

        pthread_cond_signal(&(b->cv_prod));

        pthread_mutex_unlock(&(b->mutex));

        int prod=m.values[0]*m.values[1];

        printf("[%d]- Thread worker: Prodotto pari a %d\n",getpid(),prod);
        
        struct msg_res res;
        res.product=prod;
        res.tipo=m.client_pid;

        msgsnd(queue_res,&res,sizeof(struct msg_res)-sizeof(long),0);
    }
}

void *manager(void *p){
    printf("[%d]- Thread manager: Buongiorno\n",getpid());

    struct param_manager *par=(struct param_manager *)p;

    while(1){
        struct msg_req m;
        if(msgrcv(queue_req,&m,sizeof(struct msg_req)-sizeof(long),0,IPC_NOWAIT)!=-1){
            if(m.values[0]==-1 && m.values[1]==-1){
                printf("[%d]- Thread manager: Messaggio di terminazione ricevuto\n",getpid());
                
                for(int i=0;i<NUM_WORKERS;i++){
                    pthread_cancel(par->th_w[i]);
                }

                pthread_exit(0);
            }
            
            printf("[%d]- Thread manager: ricezione del messaggio avvenuta\n",getpid());

            //produzione sul buffer
            pthread_mutex_lock(&(par->b->mutex));

            while(par->b->nelem==DIM)
                pthread_cond_wait(&(par->b->cv_prod),&(par->b->mutex));

            par->b->buf[par->b->testa]=m;
            par->b->testa=(par->b->testa+1)%DIM;
            par->b->nelem++;

            pthread_cond_signal(&(par->b->cv_cons));

            pthread_mutex_unlock(&(par->b->mutex));

            printf("[%d]- Thread manager: Produzione su buffer avvenuta\n",getpid());

        }else sleep(1);
    }
}

int main(){

    printf("[%d]- Server: Buongiorno\n",getpid());

    pthread_t th_w[NUM_WORKERS],th_m;

    __key_t req_msg_key=ftok(".",'k');
    __key_t res_msg_key=ftok(".",'j');

    //recupero code
    queue_req=msgget(req_msg_key,0);
    queue_res=msgget(res_msg_key,0);

    //instanzazione e inizializzazione buffer condiviso
    struct buf_circolare *b=(struct buf_circolare *)malloc(sizeof(struct buf_circolare));

    struct msg_req m;
    m.client_pid=0;
    m.tipo=1;
    m.values[0]=m.values[1]=0;

    for(int i=0;i<DIM;i++)
        b->buf[i]=m;

    b->coda=b->nelem=b->testa=0;

    pthread_mutex_init(&(b->mutex),NULL);
    pthread_cond_init(&(b->cv_cons),NULL);
    pthread_cond_init(&(b->cv_prod),NULL);

    //creazione threads workers
    for(int i=0;i<NUM_WORKERS;i++){
        pthread_create(&th_w[i],NULL,worker,(void *)b);
    }

    //creazione thread manager
    struct param_manager *p=(struct param_manager *)malloc(sizeof(struct param_manager));
    p->b=b;
    for(int i=0;i<NUM_WORKERS;i++)
        p->th_w[i]=th_w[i];

    pthread_create(&th_m,NULL,manager,(void *)p);
    
    //join thread manager
    pthread_join(th_m,NULL);
    printf("[%d]- Server: Thread manager terminato\n",getpid());

    //deallocazione e rimozione buffer
    pthread_cond_destroy(&(b->cv_cons));
    pthread_cond_destroy(&(b->cv_prod));
    pthread_mutex_destroy(&(b->mutex));
    free(b);
    free(p);

    return 0;
}

