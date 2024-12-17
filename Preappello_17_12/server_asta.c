#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "header.h"

// Code 
int mq_offerte;
int mq_risposte;

//l'elemento di offerte in posizione '0' non è usato in quanto i clienti
//sono numerati da 1 a MAX_CLIENTI+1
typedef struct tipo_asta {
    int offerte[MAX_CLIENTI+1]; //elemento in posizione '0' non usato

    //TODO aggiunta variabili per la sincronizzazione
    pthread_mutex_t mutex;
    pthread_cond_t cv_scrittura;
    pthread_cond_t cv_lettura;

    int num_lettori;
    int num_scrittori;
    int num_scrittori_wait;
    int num_lettori_wait;

} Asta;

typedef struct tipo_max_offerta {
    int valore_offerto;
    int id_client_offerente;
    int offerta_aggiudicata; // stato dell'offerta

    //TODO aggiunta variabili per la sincronizzazione
    pthread_mutex_t mutex;
    pthread_cond_t cv_aggiudicata;

} Max_Offerta;

// Struttura per il passaggio parametri ai thread
typedef struct tipo_parametri {
    Asta* asta;
    Max_Offerta* max_offerta;
    int client_id;
} Parametri;

// TODO: lettori-scrittori con starvation di entrambi
void InizioLettura(Asta * asta){
    pthread_mutex_lock(&(asta->mutex));
    
    while(asta->num_scrittori>0){
        asta->num_lettori_wait++;
        pthread_cond_wait(&(asta->cv_lettura),&(asta->mutex));
        asta->num_lettori_wait--;
    }
    asta->num_lettori++;
    pthread_mutex_unlock(&(asta->mutex));
}

void FineLettura(Asta * asta){
    pthread_mutex_lock(&(asta->mutex));
    asta->num_lettori--;

    if(asta->num_lettori==0) pthread_cond_signal(&(asta->cv_scrittura)); //TODO

    pthread_mutex_unlock(&(asta->mutex));
}

void InizioScrittura(Asta * asta){
    pthread_mutex_lock(&(asta->mutex));

    while(asta->num_lettori>0 || asta->num_scrittori>0){
        asta->num_scrittori_wait++;
        pthread_cond_wait(&(asta->cv_scrittura),&(asta->mutex));
        asta->num_scrittori_wait--;
    }
    asta->num_scrittori++;
    pthread_mutex_unlock(&(asta->mutex));
}

void FineScrittura (Asta * asta){
    pthread_mutex_lock(&(asta->mutex));
    asta->num_scrittori--;

    if(asta->num_scrittori_wait>0)pthread_cond_signal(&(asta->cv_scrittura));
    else pthread_cond_broadcast(&(asta->cv_lettura));

    pthread_mutex_unlock(&(asta->mutex));
}

void* banditore(void* par){
    //recupero parametri
    Parametri *p=(Parametri *)par;
    Asta *asta=(Asta *)p->asta;
    Max_Offerta *max_offerta=(Max_Offerta *)p->max_offerta;
    int client_id=(int)p->client_id;
   
    int aggiudicata = 0;
    while(!aggiudicata) {
        offerta_msg off;
        //riceve il messaggio di offerta dal proprio client_id
        msgrcv(mq_offerte,&off,sizeof(offerta_msg)-sizeof(long),client_id,0);
       
        printf("BANDITORE: Ricevuto un messaggio di offerta dal client %ld\n",off.id_cliente);
        
        //recupera da 'max_offerta' in mutua esclusione lo stato dell'offerta
        pthread_mutex_lock(&(max_offerta->mutex));

        aggiudicata=max_offerta->offerta_aggiudicata;

        pthread_mutex_unlock(&(max_offerta->mutex));       

        if (!aggiudicata) {
            // TODO risponde al cliente con lo stato dell'asta, accedendo
            // a 'max_offerta' in mutua esclusione
            risposta_offerta_msg risp;

            pthread_mutex_lock(&(max_offerta->mutex));
            risp.asta_aggiudicata=max_offerta->offerta_aggiudicata;
            risp.id_cliente=client_id;
            risp.id_vincitore=0;

            risp.max_valore_offerto=max_offerta->valore_offerto;
            pthread_mutex_unlock(&(max_offerta->mutex));

            msgsnd(mq_risposte,&risp,sizeof(risposta_offerta_msg)-sizeof(long),0);
            
            // TODO aggiorna l'array offerte come SCRITTORE usando inizio-fine scrittura
            // inserendo l'offerta ricevuta nella posizione corrispondente al client_id

            InizioScrittura(asta);

            asta->offerte[off.id_cliente]=off.valore_offerto;

            FineScrittura(asta);
           
            printf("BANDITORE: Valore offerto da %ld pari a: %d\n",off.id_cliente,off.valore_offerto);
        }
    }
    //rimozione parametro p
    free(p);

    pthread_exit(NULL);
}

void* giudice(void* par){
    //recupero parametri
    Parametri *p=(Parametri *)par;
    Asta *asta=(Asta *)p->asta;
    Max_Offerta *max_offerta=(Max_Offerta *)p->max_offerta;
    // client_id non serve a questo thread

    for (int i=0;i<TURNI;i++) {
        sleep(2);
        // TODO: Accedendo in LETTURA su 'asta' con inizio-fine lettura, determina l'offerta massima 'max_off'
        // e l'indice 'max_off_client_id' del client dell'offerta massima corrente
        
        int max_off=0;
        int max_off_client_id=-1;

        InizioLettura(asta);

        for (int j=1;j<=MAX_CLIENTI;j++){
            if(asta->offerte[j]>max_off){
                max_off=asta->offerte[j];
                max_off_client_id=j;
            }
        }
        
        FineLettura(asta);

        //Aggiornare i dati di 'max_offerta' in mutua esclusione
        pthread_mutex_lock(&(max_offerta->mutex));
        max_offerta->valore_offerto = max_off;
        max_offerta->id_client_offerente = max_off_client_id;
        pthread_mutex_unlock(&(max_offerta->mutex));
        
        printf("GIUDICE: turno %d: massimo valore correntemente offerto %d dal client %d\n",i,max_off,max_off_client_id);
    }
    
    // TODO: finiti i turni determina l'asta aggiudicata, modificando 'max_offerta'
    // in mutua esclusione e segnalando l'aggiudicatore
    //...
    pthread_mutex_lock(&(max_offerta->mutex));
    max_offerta->offerta_aggiudicata=1;
    pthread_cond_signal(&(max_offerta->cv_aggiudicata));
    pthread_mutex_unlock(&(max_offerta->mutex));

    printf("GIUDICE: asta chiusa\n");
    pthread_exit(NULL);
}

void* aggiudicatore(void* par) {
    Max_Offerta *max_offerta=(Max_Offerta *)par;

    risposta_offerta_msg risp;

    pthread_mutex_lock(&(max_offerta->mutex));

    while(max_offerta->offerta_aggiudicata==0)
        pthread_cond_wait(&(max_offerta->cv_aggiudicata),&(max_offerta->mutex));
    
    //preparazione risposta
    risp.asta_aggiudicata=1;
    risp.id_vincitore=max_offerta->id_client_offerente;
    risp.max_valore_offerto=max_offerta->valore_offerto;

    pthread_mutex_unlock(&(max_offerta->mutex));

    for (int i=1;i<MAX_CLIENTI+1;i++){
        //invio ad ogni client
        risp.id_cliente=i;
        msgsnd(mq_risposte,&risp,sizeof(risposta_offerta_msg)-sizeof(long),0);
    }
    printf("AGGIUDICATORE: asta aggiudicata a %d!\n",risp.id_vincitore);
    pthread_exit(NULL);
}

int main() {
    //inizializzazioni chiavi e code
    key_t key_queue_off=ftok(".",'k');
    key_t key_queue_risp=ftok(".",'j');
    mq_offerte=msgget(key_queue_off,0);
    mq_risposte=msgget(key_queue_risp,0);

  
    // TODO: inizializzazione parametri
    Asta *asta=(Asta *)malloc(sizeof(Asta));
    for(int i=0;i<MAX_CLIENTI+1;i++)
        asta->offerte[i]=0;

    asta->num_lettori=asta->num_scrittori=asta->num_lettori_wait=asta->num_scrittori_wait=0;        //TODO
    pthread_mutex_init(&(asta->mutex),NULL);
    pthread_cond_init(&(asta->cv_lettura),NULL);
    pthread_cond_init(&(asta->cv_scrittura),NULL);
    
    Max_Offerta *max_off=(Max_Offerta *)malloc(sizeof(Max_Offerta));
    max_off->valore_offerto=max_off->id_client_offerente=max_off->offerta_aggiudicata=0;        //TODO
    pthread_mutex_init(&(max_off->mutex),NULL);
    pthread_cond_init(&(max_off->cv_aggiudicata),NULL);

    Parametri *par=(Parametri *)malloc(sizeof(Parametri));
    par->asta=asta;
    par->max_offerta=max_off;
    par->client_id=0;

    pthread_t giudice_t;
    //TODO creazione thread giudice
    pthread_create(&giudice_t,NULL,giudice,(void *)par);
    printf("Thread giudice avviato\n");
    
    pthread_t agg_t;
    //TODO creazione thread aggiudicatore (nota: necessita solo di max_off)
    pthread_create(&agg_t,NULL,aggiudicatore,(void *)max_off);
    printf("Thread aggiudicatore avviato\n");

    pthread_t banditore_t[MAX_CLIENTI];
    //TODO creazione thread banditori
    for (int client_id=1;client_id<MAX_CLIENTI+1;client_id++) {
        // creazione parametri per il banditore
        Parametri* par_b=(Parametri *)malloc(sizeof(Parametri));  //TODO rimozione di questi Parametri, forse posso riutilizzare la struttura precedente
        par_b->asta=asta;
        par_b->client_id=client_id;
        par_b->max_offerta=max_off;

        // e creazione thread
        pthread_create(&banditore_t[client_id-1],NULL,banditore,(void *)par);
        printf("Thread banditore %d avviato\n",client_id);
    }
    
    // TODO join dei thread
    for(int i=0;i<MAX_CLIENTI;i++)
        pthread_join(banditore_t[i],NULL);

    pthread_join(agg_t,NULL);
    pthread_join(giudice_t,NULL);
    
    // e rimozione parametri, ecc.
    //rimozione asta
    pthread_mutex_destroy(&(asta->mutex));
    pthread_cond_destroy(&(asta->cv_lettura));
    pthread_cond_destroy(&(asta->cv_scrittura));
    free(asta);

    //rimozione max_off
    pthread_mutex_destroy(&(max_off->mutex));
    pthread_cond_destroy(&(max_off->cv_aggiudicata));
    free(max_off);

    //rimozione par
    free(par);

    pthread_exit(NULL);
}