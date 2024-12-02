#include "stack.h"

#include <stdlib.h>

void StackInit(Stack * s, int dim) {
	s->dim=dim;
	s->head=0;
	s->dati=(Elem *)malloc(sizeof(Elem)*dim);

	pthread_mutex_init(&(s->mutex),NULL);
	pthread_cond_init(&(s->cond_inserimento),NULL);
	pthread_cond_init(&(s->cond_prelievo),NULL);

}

void StackRemove(Stack * s) {
	for(int i=0;i<s->dim;i++)
		free(&(s->dati[i]));

	pthread_mutex_destroy(&(s->mutex));
	pthread_cond_destroy(&(s->cond_inserimento));
	pthread_cond_destroy(&(s->cond_prelievo));	
}

void StackPush(Stack * s, Elem e) {
	pthread_mutex_lock(&(s->mutex));

	while(s->head==s->dim){
		pthread_cond_wait(&(s->cond_inserimento),&(s->mutex));
	}

	s->dati[s->head]=e;
	s->head++;

	pthread_cond_signal(&(s->cond_prelievo));

	pthread_mutex_unlock(&(s->mutex));
}


Elem StackPop(Stack * s) {

	int elemento;

	pthread_mutex_lock(&(s->mutex));

	while(!s->head>0){
		pthread_cond_wait(&(s->cond_prelievo),&(s->mutex));
	}

	elemento=s->dati[s->head-1];
	s->head--;

	pthread_cond_signal(&(s->cond_inserimento));

	pthread_mutex_unlock(&(s->mutex));

	return elemento;
}

int StackSize(Stack * s) {
	int size;

	pthread_mutex_lock(&(s->mutex));
	size==s->head;
	pthread_mutex_unlock(&(s->mutex));

	return size;
}
