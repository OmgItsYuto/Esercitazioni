#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int a;

struct dati {
    char id;
    int *a;
}dati;

void *fun(void *par){
    struct dati *d=(struct dati *)par;
    printf("Ciao, sono il thread %c\n",d->id);

    for(int i=0;i<10;i++){
        printf("%c\n",d->id);
        usleep(200000);
        (*d->a)++;
    }
    pthread_exit(NULL);
}




int main(){
    int *a=malloc(sizeof(int));
    *a=0;
    pthread_t th1,th2;

    struct dati *d1=malloc(sizeof(struct dati));
    d1->id='a';
    d1->a=a;

    struct dati *d2=malloc(sizeof(struct dati));
    d2->id='b';
    d2->a=a;

    pthread_create(&th1,NULL,fun,(void *)d1);
    pthread_create(&th2,NULL,fun,(void *)d2);

    pthread_join(th1,NULL);
    pthread_join(th2,NULL);

    printf("a: %d\n",*a);

    free(a);
    free(d1);
    free(d2);
    //pthread_exit(NULL);
}