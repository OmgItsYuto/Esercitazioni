#ifndef _COMMON_
#define _COMMON_

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_DIM 10

#define END_MESSAGE 1
#define CLIENT_SERVER_MESSAGE 2
#define SERVER_PRINTER_MESSAGE 3

/* Definizione Tipi di Messaggi*/
typedef struct{
    long tipo;
    int processo;
}msg_client;

/* Definizione Messaggio Richiesta del Server, contenente il buffer */
typedef struct{
    long tipo;
    int list[BUFFER_DIM];
} msg_server;



/* Prototipi delle funzioni dei processi */
void printer(int msgq_print);
void server(int msgq_guest, int msgq_print);
void client(int msgq_guest);

#endif
