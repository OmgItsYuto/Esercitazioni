#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int* var;

void fun(char a, int i){
    printf("Sono il processo %c\n",a);
    for(int j=0;j<i;j++){
        var+=2;
        int b=*var*j;
        (*var)--;
    }
    printf("Valore finale di var per %c è %d\n",a,var);
}

#define N 3

int main(){
    __pid_t pid;
    var=malloc(sizeof(int));
    *var=0;
    for(int i=0; i<N; i++){
        char a='a'+i;
        pid=fork();

        if(pid==0){
            fun(a,10);
            exit(0);
        }
    }
    for (int i = 0; i < 3; i++){
        wait(NULL);
    }
    
    printf("Valore finale di var per il main è %d\n",var);

    return 0;
}