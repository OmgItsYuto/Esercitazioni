#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void *Inserisci(void * s)
{
	int i;
	Elem v;
    
    Stack * stack = (Stack *) s;

	for(i=0; i<4; i++) {
		v = rand() % 11;
		StackPush(stack, v);
		printf("Inserimento: %d\n", v);
		sleep(1);
	}

	// Terminazione thread
	pthread_exit(NULL);
}


void *Preleva(void * s)
{
	int i;
	Elem v1, v2;

    Stack * stack = (Stack *) s;
    
	for(i=0; i<10; i++) {
		v1=StackPop(stack);
		printf("Prelievo: %d\n", v1);

		v2=StackPop(stack);
		printf("Prelievo: %d\n", v2);

		printf("Somma: %d\n", v1+v2);

		sleep(3);
	}

	// Terminazione thread
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	// Istanziamo i thread
	pthread_t th[NUM_THREAD_INSERIMENTO+NUM_THREAD_PRELIEVO];
	int i,dimension=10;

	srand(time(NULL));

	printf("Riga 58\n");

	// Istanziamo lo stack
	Stack *stack=(Stack *)malloc(sizeof(Stack));
	printf("Riga 62\n");

	StackInit(stack,dimension);

	// Attributi thread
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for(i=0; i<NUM_THREAD_INSERIMENTO; i++) {
		pthread_create(&th[i],&attr,Inserisci,(void *)stack);
	}

	// Creazione thread per il prelievo
	pthread_create(&th[NUM_THREAD_INSERIMENTO],&attr,Preleva,(void *)stack);

	for(i=0; i<6; i++) {
		// Attesa terminazione thread
		pthread_join(th[i],NULL);
	}


	// Rimozione stack
	free(stack);

	pthread_exit(0);
}

