#include <stdio.h>
#include <unistd.h>

int main(){
    int pid=fork();

    if (pid==0){
        printf("PID==0:%d\n",getpid());
    }else{
        printf("PID:%d\n",pid);
    }
    


    return 0;
}