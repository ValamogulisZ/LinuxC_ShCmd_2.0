/***
 * Name: 	Wenda Zhao
 * BlazerId: 	wenda
 * Project #: 	2
 * Comment: 	I used code from lab7 execvp code as reference
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define RUN_SH 1
#define QUIT_SH 0

void createarray(char *buf, char **array) {
	int i, count;
	int blank_count;

	buf[strlen(buf)] = '\0';
	
	for (i = 0, array[0] = &buf[0], count = 1; i < strlen(buf); i++) {
		if (buf[i] == ' ') {
		   buf[i] = '\0';
		   array[count++] = &buf[i+1];
		}
	}
	array[count] = (char *)NULL;
}

int main(int argc, char **argv) {
	pid_t pid;
	int status;
	char input_command[BUFSIZ], buf[BUFSIZ], *args[BUFSIZ];
	
	int run_flag = RUN_SH;
	const char *quit_str = "quit";
	const char *help_str = "help";
	const char *list_str = "list";
	const char *history_str = "history";
	const char *enter = "\n";
	char log;
	
	FILE *fp;
    
	
	while(run_flag){
		printf("blazersh> ");
		gets(input_command);
		// printf("Commond get: %s\n", input_command);
		fp = fopen("output.log","a");
		if (fp == NULL) {
			printf("Error opening file output.log for writing\n");
			exit(-1);
		}
		fwrite(input_command, strlen(input_command), 1, fp);
		fwrite(enter, strlen(enter), 1, fp);
		fclose(fp);
		
		createarray(input_command, args);
		
		pid = fork();
		if (pid == 0) {
			if (strcmp(args[0], quit_str)==0){
				run_flag = QUIT_SH;
				exit(0);
			}
			
			else if(strcmp(args[0], history_str)==0){
				fp = fopen("output.log","r");
				if (fp == NULL) {
					printf("Error opening file output.log for writing\n");
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
				// printf("args[0]: %s\n", args[0]);
				execvp(args[0], args);
				perror("exec");
				exit(-1);
			}
		} 
		
		else if (pid > 0) {
			// printf("Wait for the child process to terminate\n");
			if (strcmp(args[0], quit_str)==0){
				run_flag = QUIT_SH;
				printf("Now quit blazersh...");
				exit(0);
			}
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
			printf("Now quit blazersh...");
			exit(0);
		}
		else if(strcmp(args[0], help_str)==0){
			printf("***********HELP*****************\n");
			printf("Use any leagal command.\n");
			printf("Such as: pwd\t hostname\t ls..\n");
			printf("To quit blazersh, enter quit.\n");
			printf("Author's contact email:\n");
			printf("\t wenda@uab.edu\n");
		}
		
	}

    printf("[%ld]: Exiting main program .....\n", (long)getpid());

    return 0;	
}
