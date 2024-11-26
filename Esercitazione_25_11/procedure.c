#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "procedure.h"

void utente(BufferCircolare *buf){

    printf("[%d] - Utente: Buongiorno\n",getpid());


    for(int i=0;i<N_RICHIESTEPERUTENTE;i++){
        srand(time(NULL)*getpid());

        richiesta r;
        r.processo=getpid();
        r.posizione=rand()%19;

        printf("[%d] - Utente: Accesso al monitor...\n",getpid());
        enter_monitor(&(buf->m));
        printf("[%d] - Utente: Entrato nel monitor...\n",getpid());

        printf("[%d] - Utente: Il numero di elementi nel buffer è %d\n",getpid(),buf->cnt);

        if(buf->cnt==N){
            printf("[%d] - Utente: Attendo che il buffer si liberi\n",getpid());
            wait_condition(&(buf->m),0);
            printf("[%d] - Utente: Il buffer si è liberato\n",getpid());
        }

        buf->elementi[buf->coda]=r;
        buf->coda=(buf->coda+1)%N;
        buf->cnt++;

        sleep(1);

        signal_condition(&(buf->m),1);

        printf("[%d] - Utente: Uscita dal monitor...\n",getpid());
        leave_monitor(&(buf->m));
    }
}

void schedulatore(BufferCircolare *buf, pid_t *disco){

    printf("[%d] - Schedulatore: Buongiorno\n",getpid());

    for(int i=0;i<N_RICHIESTEPERUTENTE*N_UTENTI;i++){
        int pos_prece=0;

        printf("[%d] - Schedulatore: Accesso al monitor...\n",getpid());
        enter_monitor(&(buf->m));

        if(buf->cnt==0){
            printf("[%d] - Schedulatore: Il buffer è vuoto, attendo richieste\n",getpid());
            wait_condition(&(buf->m),1);
            printf("[%d] - Schedulatore: Sono presenti nuove richieste nel buffer\n",getpid());
        }
        
        richiesta r=buf->elementi[buf->testa];

        sleep(abs(r.posizione-pos_prece));

        printf("[%d] - Schedulatore: Scrivo il valore %d sul disco, in posizione %d\n",getpid(),r.processo,r.posizione);
        disco[r.posizione]=r.processo;
        buf->testa=(buf->testa+1)%N;
        buf->cnt--;

        pos_prece=r.posizione;

        signal_condition(&(buf->m),0);

        printf("[%d] - Schedulatore: Uscita dal monitor...\n",getpid());
        leave_monitor(&(buf->m));
    }
}