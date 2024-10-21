#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

int* var;

void fun(char a, int i){
    printf("Sono il processo %c\n",a);
    for(int j=0;j<i;j++){
        *var+=2;
        int b=*var*j;
        (*var)--;
    }
    printf("Valore finale di var per %c è %d\n",a,*var);
}

#define N 3

int main(){
    __pid_t pid;
    int shm=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0664);
    var=shmat(shm,NULL,0);
    *var=0;
    for(int i=0; i<N; i++){
        char a='a'+i;
        pid=fork();

        if(pid==0){
            fun(a,100000);
            exit(0);
        }
    }
    for (int i = 0; i < 3; i++){
        wait(NULL);
    }
    shmctl(shm,IPC_RMID,0);
    printf("Valore finale di var per il main è %d\n",*var);

    return 0;
}