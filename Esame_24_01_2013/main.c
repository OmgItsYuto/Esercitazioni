#include "header.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

void *cliente(void *par){
    printf("[%d] - Thread cliente iniziato\n",getpid());
    struct Monitor *m=(struct Monitor *)par;

    int copia;

    for(int i=0;i<3;i++){
        printf("Richiesta di affitto per il film %d\n",i+1);
        copia=affitta(m,i+1);
        sleep(2);
        restituisci(m,i+1,copia);
        printf("Restituzione effettuata per il film %d\n",i+1);
    }

    pthread_exit(0);
}

void *printer(void *par){
    printf("[%d] - Thread printer iniziato\n",getpid());
    struct Monitor *m=(struct Monitor *)par;

    for(int i=0;i<20;i++){
        stampa(m);
        sleep(1);
    }

    pthread_exit(0);
}

int affitta(struct Monitor * m, int id_film){
    int index=0,copia;
    
    pthread_mutex_lock(&(m->mutex));

    while(m->disp_cnt[id_film-1]==0)
        pthread_cond_wait(&(m->cond_affitto[id_film-1]),&(m->mutex));

    while(index<NUMDVD && (m->dvd[index].stato==AFFITTATO || m->dvd[index].identificativo_film!=id_film)){
        index++;
    }

    m->disp_cnt[id_film-1]--;
    m->dvd[index].stato=AFFITTATO;
    copia=m->dvd[index].identificativo_copia;

    pthread_mutex_unlock(&(m->mutex));

    printf("Richiesta d'affitto per il film %d andata a buon fine\n",id_film);   

    return copia;
}

void restituisci(struct Monitor * m, int id_film, int id_copia){
    int index=0;

    pthread_mutex_lock(&(m->mutex));

    while(index<NUMDVD && (m->dvd[index].identificativo_film!=id_film || m->dvd[index].identificativo_copia!=id_copia)){
        index++;
    }

    m->dvd[index].stato=DISPONIBILE;
    m->disp_cnt[id_film-1]++;

    if(m->disp_cnt[id_film-1]==2)
        pthread_cond_signal(&(m->cond_affitto[id_film-1]));

    pthread_mutex_unlock(&(m->mutex));
}

void stampa(struct Monitor * m){
    pthread_mutex_lock(&(m->mutex));

    printf("Stampa DVD nel negozio:\n");
    for(int i=0;i<NUMDVD;i++){
        printf("DVD #%d {Id_film: %d, Id_copia: %d, Stato: %d}\n",i,m->dvd[i].identificativo_film,m->dvd[i].identificativo_copia,m->dvd[i].stato);
    }

    pthread_mutex_unlock(&(m->mutex));
}

int main(){
    printf("[%d] - Main iniziato\n",getpid());

    pthread_t th[NUMCLIENTI+NUMPRINTERS];

    //creazione e allocazione strutture
    struct Monitor *monitor_dvd=(struct Monitor *)malloc(sizeof(struct Monitor));
     
    for(int i=0;i<NUMDVD;i++){
        monitor_dvd->dvd[i].identificativo_film=(i%3)+1;
        monitor_dvd->dvd[i].identificativo_copia=(i%2)+1;
        monitor_dvd->dvd[i].stato=0;
    }

    for(int i=0;i<3;i++)
        monitor_dvd->disp_cnt[i]=2;

    pthread_mutex_init(&(monitor_dvd->mutex),NULL);

    for(int i=0;i<3;i++)
        pthread_cond_init(&(monitor_dvd->cond_affitto[i]),NULL);

    //creazione threads
    for(int i=0;i<NUMCLIENTI;i++)
        pthread_create(&th[i],NULL,cliente,(void *)monitor_dvd);

    pthread_create(&th[NUMCLIENTI],NULL,printer,(void *)monitor_dvd);

    //join threads
    for(int i=0;i<NUMCLIENTI+NUMPRINTERS;i++)
        pthread_join(th[i],NULL);

    //deallocazione delle strutture create
    pthread_mutex_destroy(&(monitor_dvd->mutex));
    for(int i=0;i<3;i++){
        pthread_cond_destroy(&(monitor_dvd->cond_affitto[i]));
    }
    free(monitor_dvd);

    pthread_exit(0);
}