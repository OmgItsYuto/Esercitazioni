#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "semafori.h"

#define MUTEX 0

int* var;

void fun(char a, int i, int semid){
    printf("Sono il processo %c\n",a);
    for(int j=0;j<i;j++){
        Wait_Sem(semid,MUTEX);
        *var+=2;
        int b=*var*j;
        (*var)--;
        Signal_Sem(semid,MUTEX);
    }
    printf("Valore finale di var per %c è %d\n",a,*var);
}

#define N 3

int main(){
    __pid_t pid;
    int shm=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0664);
    var=shmat(shm,NULL,0);
    *var=0;

    int semid=semget(IPC_PRIVATE,1,IPC_CREAT|0644);
    semctl(semid,MUTEX,SETVAL,1);

    for(int i=0; i<N; i++){
        char a='a'+i;
        pid=fork();

        if(pid==0){
            fun(a,150000,semid);
            exit(0);
        }
    }
    for (int i = 0; i < 3; i++){
        wait(NULL);
    }
    shmctl(shm,IPC_RMID,0);
    semctl(semid,0,IPC_RMID,0);
    printf("Valore finale di var per il main è %d\n",*var);

    return 0;
}