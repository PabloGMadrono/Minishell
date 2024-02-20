// P2-SSOO-22/23

// MSH main file
// Write your msh source code here

//#include "parser.h"
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h> /* NULL */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#define MAX_COMMANDS 8

// files in case of redirection
char filev[3][64];

// to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param) {
  printf("****  Exiting MSH **** \n");
  // signal(SIGINT, siginthandler);
  exit(0);
}
void signstphandler(int param) {
  printf("Hello I'm the handler for stopped processes\n");
  exit(0);
}

/* Timer */
pthread_t timer_thread;
unsigned long mytime = 0;

void *timer_run() {
  while (1) {
    usleep(1000);
    mytime++;
  }
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char ***argvv, int num_command) {
  // reset first
  for (int j = 0; j < 8; j++)
    argv_execvp[j] = NULL;

  int i = 0;
  for (i = 0; argvv[num_command][i] != NULL; i++)
    argv_execvp[i] = argvv[num_command][i];
}

/**
 * Main sheell  Loop
 */
int main(int argc, char *argv[]) {
  /**** Do not delete this code.****/
  int end = 0;
  int executed_cmd_lines = -1;
  char *cmd_line = NULL;
  char *cmd_lines[10];
  //Create environmental variable for mycalc
  setenv("Acc", "0", 1);

  if (!isatty(STDIN_FILENO)) {
    cmd_line = (char *)malloc(100);
    while (scanf(" %[^\n]", cmd_line) != EOF) {
      if (strlen(cmd_line) <= 0)
        return 0;
      cmd_lines[end] = (char *)malloc(strlen(cmd_line) + 1);
      strcpy(cmd_lines[end], cmd_line);
      end++;
      fflush(stdin);
      fflush(stdout);
    }
  }

  pthread_create(&timer_thread, NULL, timer_run, NULL);

  /*********************************/

  char ***argvv = NULL;
  int num_commands;
  
  while (1) {
    int status = 0;
    int command_counter = 0;
    int in_background = 0;
    signal(SIGINT, siginthandler);

    // Prompt
    write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

    // Get command
    //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN
    // NORMAL/CORRECTION MODE***************
    executed_cmd_lines++;
    if (end != 0 && executed_cmd_lines < end) {
      command_counter = read_command_correction(&argvv, filev, &in_background,
                                                cmd_lines[executed_cmd_lines]);
    } else if (end != 0 && executed_cmd_lines == end) {
      return 0;
    } else {
      command_counter =
          read_command(&argvv, filev, &in_background); // NORMAL MODE
    }
    //************************************************************************************************

    /************************ STUDENTS CODE ********************************/
    if (command_counter > 0) {
      int file;
      if (command_counter > MAX_COMMANDS) {
        printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
      
      }
      //Check for mycalc
      else if ((strcmp(argvv[0][0], "mycalc")) == 0){
      //Create fork
			int pid, aux;
      pid = fork();

			switch(pid){
				case -1:
				  perror("Some error occured");
          break;
				case 0:
          //Exit child because we are doing internal command
          exit(0);
							

				default:
        //Wait for child and check arguments are correct
				while (wait(&status) != pid); 
        if ((argvv[0][1]) && (argvv[0][2]) && (argvv[0][3]) && (argvv[0][4] == NULL)){
          //Check to see if atoi gives 0 because integer 0 is given or because letters are given 
          int par1, par2;
          par1 = atoi(argvv[0][1]);
          par2 = atoi(argvv[0][3]);
          if (par1 == 0 && ((strcmp(argvv[0][1], "0") != 0))){
            printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");  
            break;         
          }
          if (par2 == 0 && ((strcmp(argvv[0][3], "0") != 0))){
            printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
            break;
            
          }
          else{      
            //Choosing between add mul or div    
          if ((strcmp(argvv[0][2], "add")) == 0){							
          char acc_aux[10];
          //Setting up environmental variable
          aux = atoi(getenv("Acc"));        
          aux = aux + par1 + par2;
          sprintf(acc_aux, "%d", aux);
          setenv("Acc", acc_aux, 1);
          //Printing the result
          fprintf(stderr, "[OK] %d + %d = %d; Acc %d\n ", atoi(argvv[0][1]), atoi(argvv[0][3]), atoi(argvv[0][1]) + atoi(argvv[0][3]), aux);
          }
          else if ((strcmp(argvv[0][2], "mul")) == 0){							
            //Result for mul
            fprintf(stderr, "[OK] %d * %d = %d\n ", atoi(argvv[0][1]), atoi(argvv[0][3]), atoi(argvv[0][1]) * atoi(argvv[0][3]));
          }
          else if ((strcmp(argvv[0][2], "div")) == 0){							
            //Result for div  
            fprintf(stderr, "[OK] %d / %d = %d; Remainder %d\n", atoi(argvv[0][1]),atoi(argvv[0][3]), atoi(argvv[0][1]) / atoi(argvv[0][3]),atoi(argvv[0][1]) % atoi(argvv[0][3]));
            
          }     
          else{
            //If none of the three operations are done, error
            printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
          }
          }
        }
        else{
          //We get to this error if the arguments are not correct 
          printf("[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
         
        }
        

			}


		}
      //Check for mytime
		  else if ((strcmp(argvv[0][0], "mytime")) == 0){
			//Create fork
      int pid;
			pid = fork();
      //Variables used for making the HH:MM:SS format and the calculations
			int myhours = 0, mymins = 0, mysecs = 0;
      char mysecss[3], myminss[3], myhourss[3];

			switch(pid){
				case -1:
				  perror("Some error occured");
				  break;
				case 0:
          //As we are doing an internal command we exit the child 
          exit(0);
							

				default:
          //Exit child
					while (wait(&status) != pid); 
          //Make calculations to get hours, minutes and seconds
          mysecs = mytime/1000;
					if (mysecs > 59){
						mymins = mysecs/60;
						mysecs = mysecs%60;

					}
					if (mymins > 59){
						myhours = mymins/60;
						mymins = mymins%60;
					}
          //This is so that HH:MM:SS format is displayed
          sprintf(mysecss, "%d", mysecs);
          sprintf(myminss, "%d", mymins);
          sprintf(myhourss, "%d", myhours);
          if (strlen(mysecss) < 2){
                
            sprintf(mysecss, "0%d", mysecs);
          }
          if (strlen(myminss) < 2){
            sprintf(myminss, "0%d", mymins);
          }
          if (strlen(myhourss) < 2){
                
            sprintf(myhourss, "0%d", myhours);
          }
					fprintf(stderr, "%s:%s:%s\n", myhourss, myminss, mysecss);


				}
			}
      
      else{
        // Create variables for fork and for the pipes        
        int pid, status;
        int num_pipes = (command_counter > 1) ? command_counter - 1 : 0;
        int pipes[num_pipes][2];

        // Create pipes
        for (int i = 0; i < num_pipes; i++) {
          if (pipe(pipes[i]) == -1) {
            printf("Failed to create pipe\n");
            return -1;
          }
        }

        // Execute commands
        for (int i = 0; i < command_counter; i++) {
          //Create the childs 
          pid = fork();
          if (pid == 0) {
            // Child process

            //Close all pipes 
            for (int j = 0; j < num_pipes; j++) {
              if (j != i && j != i - 1) {
                close(pipes[j][0]);
                close(pipes[j][1]);
              }
            }

            if (num_pipes == 0) {
              // If 0 pipes, this means only one command has been issued
              if (strcmp(filev[0], "0") != 0){
                //Redirect file into command
                printf("I'm going to read input from file: %s\n", filev[0]);
                if ((file = open(filev[0], O_RDONLY, 0666)) < 0){return(-1);}
                dup2(file, STDIN_FILENO);
                close(file);
              }

              if (strcmp(filev[1], "0") != 0){
                //Redirect output into file
                printf("I'm going to create or replace output file: %s\n", filev[1]);
                if ((file = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){return(-1);}
                dup2(file, STDOUT_FILENO);
                close(file);
              }

              if (strcmp(filev[2], "0") != 0){
                //Create or replace error file
                printf("I'm going to create or replace error file: %s\n", filev[2]);
                if ((file = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){return(-1);}
                dup2(file, STDERR_FILENO);
                close(file);
              }
              //Executing
              execvp(argvv[i][0], argvv[i]);
              perror("If the execution is correct, we will never get to this\n");
            } else if (i == 0) {
              // For n pipes, we need two particular cases, for the first and last commands
              if (strcmp(filev[0], "0") != 0){
                //Redirecting file content into command
                printf("I'm going to read input from file: %s\n", filev[0]);
                if ((file = open(filev[0], O_RDONLY, 0666)) < 0){return(-1);}
                dup2(file, STDIN_FILENO);
                close(file);
              }
              if (strcmp(filev[2], "0") != 0){
                //Replacing error file 
                printf("I'm going to create or replace error file: %s\n", filev[2]);
                if ((file = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){return(-1);}
                dup2(file, STDERR_FILENO);
                close(file);
              }
              close(pipes[i][0]);
              dup2(pipes[i][1], STDOUT_FILENO);
              close(pipes[i][1]);
            } else if (i == command_counter - 1) {
              // Last command particular case
              close(pipes[i - 1][1]);
              dup2(pipes[i - 1][0], STDIN_FILENO);
              close(pipes[i - 1][0]);
              if (strcmp(filev[1], "0") != 0){
                //Redirect output to file
                printf("I'm going to create or replace output file: %s\n", filev[1]);
                if ((file = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){return(-1);}
                dup2(file, STDOUT_FILENO);
                close(file);
              }
            } else {
              // Middle command
              close(pipes[i - 1][1]);
              dup2(pipes[i - 1][0], STDIN_FILENO);
              close(pipes[i - 1][0]);
              close(pipes[i][0]);
              dup2(pipes[i][1], STDOUT_FILENO);
              close(pipes[i][1]);
            }
            //After all pipes
            execvp(argvv[i][0], argvv[i]);
            perror("If the execution is correct, we will never get to this\n");
          }
           else if (pid == -1) {
            //Error case
            perror("Some error occured");
            return -1;
          }
        }

        // Close pipes and wait for children
        for (int i = 0; i < num_pipes; i++) {
          close(pipes[i][0]);
          close(pipes[i][1]);
        }
        //Father
        for (int i = 0; i < command_counter; i++) {
        //background check
        if (in_background != 1){
          
          waitpid(pid, &status, WUNTRACED);
          }
        }
      }
    }
  }
  return 0;
}