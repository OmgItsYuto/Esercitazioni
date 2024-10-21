#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
	char buffer[128];
	pid_t pid;
	int n;
	char* args[10];
	char* token;
	bool bckgrnd, pipebool;


	while(1){
		bckgrnd=false;

		printf("Shell> ");
		fgets(buffer, 127, stdin);

		buffer[strlen(buffer) - 1] = '\0';

		printf("buffer='%s'\n", buffer);

		//tokenizzazione del comando
		token = strtok(buffer, " ");

		args[0] = token;
		printf("ARG 0: '%s'\n", args[0]);

		//tokenizzazione degli argomenti
		n = 1;
		while(token != NULL && n<10) {

			token = strtok(NULL, " ");

			args[n] = token;
			printf("ARG %d: '%s'\n", n, args[n]);

			n++;
		}

		args[9] = (char *)0;

		if(!strcmp(args[0], "exit")){
			return 0;
		}

		if(*(args[n-2])=='&'){
			bckgrnd=true;
			(args[n-2])=NULL;
		}

		pid = fork();

		if(pid == -1){
			//printf("Errore: fork non riuscita!\n");
			perror("Errore: fork non riuscita!");
			return 0;
		}
		else if(pid == 0){
			int pipepointer;
			for(int i=0;i<n-1;i++){
				if(*(args[i])=='>'){
					freopen(args[i+1],"w",stdout);
					(args[i])=NULL;
					(args[i+1])=NULL;
					break;
				}

				if(*(args[i])=='|'){
					pipebool=true;
					pipepointer=i;
					break; //i is the position of the pipe operator
				}
			}


			if(pipebool){
				int pid2=fork();
				int p[2];

				if(pid2 == -1){
					//printf("Errore: fork non riuscita!\n");
					perror("Errore: fork non riuscita!");
					return 0;//exit(0);
				}
				else if(pid2 == 0){
					
					char* args2[10];

					if (pipe(p) < 0) exit(1);

					dup2(p[1], 1);  // send stdout to the pipe
    				dup2(p[1], 2);

					close(p[1]);

					for(int i=0;i<pipepointer;i++){
						args2[i]=args[i];
					}

					execvp(args[0], args2);
					//printf("Errore: exec non riuscita!\n");
					perror("Errore: exec non riuscita!");
				}else{
					fgets(buffer, 127, stdin);

					buffer[strlen(buffer) - 1] = '\0';

					printf("buffer='%s'\n", buffer);

					//tokenizzazione del comando
					token = strtok(buffer, " ");

					args[0] = token;
					printf("ARG 0: '%s'\n", args[0]);

					//tokenizzazione degli argomenti
					n = 1;
					while(token != NULL && n<10) {

						token = strtok(NULL, " ");

						args[n] = token;
						printf("ARG %d: '%s'\n", n, args[n]);

						n++;
					}
					pid2 = wait(NULL);
				}
			}

			execvp(args[0], args);
			//printf("Errore: exec non riuscita!\n");
			perror("Errore: exec non riuscita!");			
		}
		else if(!bckgrnd){
			pid = wait(NULL);
		}
	}

	return 0;
}

/*
TODO:
	background processes --> v
	output to file  freopen() --> v
	pipeline
	
*/
