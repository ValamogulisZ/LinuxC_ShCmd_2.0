/***
 * Name: 	Wenda Zhao
 * BlazerId: 	wenda
 * Project #: 	3
 * Comment: 	I used code of execvp.c, signal.c and iorederict.c from lab as references
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define RUN_SH 1
#define QUIT_SH 0

int i, count;
pid_t pid;

static void sig_usr(int signo) {
	switch(signo) {
	   case SIGINT: 
			printf("received SIGINT signal. interapting child process.. %d, pid = [%ld]\n", signo, (long)getpid());
			printf("blazersh> ");
			break;
	   case SIGTSTP:
			printf("received SIGTSTP signal. stoping child process.. %d, pid = [%ld]\n", signo, (long)getpid());
			if (pid == 0){
				kill(pid, SIGTSTP);
				exit(0);
			} else {
				kill(pid, SIGCONT);
			}
			printf("blazersh> ");
			break;
		case SIGQUIT:
			printf("received SIGQUIT signal. quiting blazersh.. %d, pid = [%ld]\n", signo, (long)getpid());
			kill(pid, SIGQUIT);
	}
}

void createarray(char *buf, char **array) {
	buf[strlen(buf)] = '\0';
	/*
	for (i = 0, array[0] = &buf[0], count = 1; i < strlen(buf); i++) {
		if (buf[i] == ' ') {
		   buf[i] = '\0';
		   array[count++] = &buf[i+1];
		}
	}
	array[count] = (char *)NULL;
	*/
	
	char *p = strtok(buf, " ");
	i = 0;
	while(p != NULL){
		array[i++] = p;
		p = strtok(NULL, " ");
	}
	count = i;
	array[count] = (char *)NULL;
	
}

int main(int argc, char **argv) {
	int status;
	int fdin, fdout;
	int ioredirect_flag;
	ioredirect_flag = 0;
	char input_command[BUFSIZ], buf[BUFSIZ], *args[BUFSIZ];
	
	int run_flag = RUN_SH;
	const char *quit_str = "quit";
	const char *help_str = "help";
	const char *list_str = "list";
	const char *history_str = "history";
	const char *input_str = "<";
	const char *output_str = ">";
	const char *enter = "\n";
	char log;
	
	FILE *fp;
    
	
	while(run_flag){
		printf("blazersh> ");
		gets(input_command);
		//printf("%s", input_command);
		//if(input_command == "\0"){
		//	continue;
		//}
		// printf("Commond get: %s\n", input_command);
		fp = fopen("blazersh.log","a");
		if (fp == NULL) {
			printf("Error opening file blazersh.log for writing\n");
			exit(-1);
		}
		fwrite(input_command, strlen(input_command), 1, fp);
		fwrite(enter, strlen(enter), 1, fp);
		fclose(fp);
		
		createarray(input_command, args);
		//printf("count = %d\n", count);
		for (i=0; i<count; i++){
			//printf("%s\n", args[i]);
			if(strcmp(args[i],input_str)==0){
				ioredirect_flag = 1;
				//printf("args[i+1] = %s", args[i+1]);
				if ((fdin = open(args[i+1], O_RDONLY)) == -1) {
				   printf("Error opening file %s for input\n", args[i+1]);
				   exit(-1);
				}
			}
		}
		
		for (i=0; i<count; i++){
			if(strcmp(args[i],output_str)==0){
				ioredirect_flag = 1;
				//printf("args[i+1] = %s", args[i+1]);
				if ((fdout = open(args[i+1], O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1) {
				   printf("Error opening file %s for output\n", args[i+1]);
				   exit(-1);
				}
			}
		}
		
		
		
		
		pid = fork();
		if (pid == 0) {
			if (strcmp(args[0], quit_str)==0){
				run_flag = QUIT_SH;
				exit(0);
			}
			
			else if(strcmp(args[0], history_str)==0){
				fp = fopen("blazersh.log","r");
				if (fp == NULL) {
					printf("Error opening file blazersh.log for writing\n");
					exit(-1);
				}
				while((log=fgetc(fp))!=EOF){
					putchar(log);
				}
				putchar('\n');
				fclose(fp);
			}
			
			else{
				if (strcmp(input_command, list_str)==0){
					args[0] = "ls";
				}
				if(ioredirect_flag == 1){
					dup2(fdin, 0);
					dup2(fdout, 1);
				}
				if (signal(SIGINT, sig_usr) == SIG_ERR) {
					printf("can't catch SIGINT\n");
					exit(-1);
				}
				if (signal(SIGTSTP, sig_usr) == SIG_ERR) {
					printf("can't catch SIGTSTP\n");
					exit(-1);
				}
				if (signal(SIGQUIT, sig_usr) == SIG_ERR) {
					printf("can't catch SIGQUIT\n");
					exit(-1);
				}
				execvp(args[0], args);
				perror("exec");
				exit(-1);
			}
			
		} 
		
		else if (pid > 0) {
			if (strcmp(args[0], quit_str)==0){
				run_flag = QUIT_SH;
				printf("Now quit blazersh...\n");
				exit(0);
			}
			
			// printf("Wait for the child process to terminate\n");
			wait(&status);
			if (WIFEXITED(status)) {
				printf("Child process exited with status = %d\n", WEXITSTATUS(status));
			} 
			else {
				printf("Child process did not terminate normally!\n");
			}
		}
		
		else {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		
		if (strcmp(args[0], quit_str)==0){
			run_flag = QUIT_SH;
			printf("Now quit blazersh...(If it's not quiting, please enter quit again)\n");
			exit(0);
		}
		else if(strcmp(args[0], help_str)==0){
			printf("***********HELP*****************\n");
			printf("Use any leagal command.\n");
			printf("Such as: pwd\t hostname\t ls..\n");
			printf("To quit blazersh, enter quit.\n");
			printf("To stop child process, press Ctrl-Z.\n");
			printf("To interupt child process, press Ctrl-C.\n");
			printf("To quit blazersh, press Ctrl-\\.\n");
			printf("To show the input history, enter history.\n");
			printf("Author's contact email:\n");
			printf("\t wenda@uab.edu\n");
		}
		
	}

    printf("[%ld]: Exiting main program .....\n", (long)getpid());

    return 0;	
}
