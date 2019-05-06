#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>

#include <sys/wait.h>

#define BUF_SIZE 1024

//----------------------------------------------------------------
// Global Data
//----------------------------------------------------------------

const char * USAGE = "USAGE loggingShell [logfile]\n";
int logFD;

int readyToExit = 0;
int curChildPID = -2;

int pipeFDs[2];

//----------------------------------------------------------------
// Builtin Function Paths
//----------------------------------------------------------------

const char * ls = "/bin/ls";
const char * pwd = "/bin/pwd";

//----------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------

void logString(const char * msg, size_t bytes);

//----------------------------------------------------------------
// Functions
//----------------------------------------------------------------

static void sigIntHandler(int sig){

  if (curChildPID == -2){
    logString("No child process executing\n", 27);
    logString("$ ", 2);
    return;
  }

  write(pipeFDs[1], "terminate child\n",16);
  char childPIDString[5];
  sprintf(childPIDString, "%d", curChildPID);

  // Send termination signal to child process
  logString("Terminating child process ", 26);
  logString(childPIDString, 5);
  logString("\n", 1);
  kill(curChildPID, SIGINT);

}

void logString(const char * msg, size_t bytes){

  write(logFD, msg, bytes);
  write(STDOUT_FILENO, msg, bytes);

}

void checkBuiltin(const char * cmd){

  // TODO
}

void processCmd(const char * cmd, ssize_t len){

  logString("Processing command: ", 20);
  logString(cmd, len);
  logString("\n", 1);


  if (strcmp(cmd, "exit") == 0){
    readyToExit = 1;
  }
  else{

    if (strcmp(cmd, "ls") == 0){
      cmd = ls;
      len = strlen(ls);
    }
    if (strcmp(cmd, "pwd") == 0){
      cmd = pwd;
      len = strlen(pwd);
    }


    if (pipe(pipeFDs) == -1){
      perror("pipe");
      return;
    }

    // Spawn a new process
    curChildPID = fork();

    if (curChildPID == -1){
      logString("Could not run program\n", 22);
    }
    else if (curChildPID == 0){

      // Redirect output to the pipe
      dup2(pipeFDs[1], STDOUT_FILENO);
      close(pipeFDs[0]);
      // close(pipeFDs[1]);

      int res = execl(cmd, cmd, NULL);

      if (res == -1){
        printf("exec failed\n");
        write(pipeFDs[1], "exec failed\n", 12);
        _exit(-1);
      }

    }
    else{

      // Read child process's output from pipe
      // close(pipeFDs[0]);
      ssize_t bytesRead;
      char childOutput[BUF_SIZE];

      char childPIDString[5];
      sprintf(childPIDString, "%d", curChildPID);

      logString("\n+++ Child ", 11);
      logString(childPIDString, 5);
      logString(" executes ", 10);
      logString(cmd, len);
      logString(" +++ \n", 6);

      // Get Child process's output
      bytesRead = read(pipeFDs[0], childOutput, BUF_SIZE - 1);
      childOutput[bytesRead] = '\0';
      logString(childOutput, bytesRead);
      memset(childOutput, 0x0, BUF_SIZE);

      int status;
      waitpid(curChildPID, &status, 0);
      logString("+++ Child ", 10);
      logString(childPIDString, 5);
      logString(" finished executing ", 20);
      logString(cmd, len);
      logString(" +++ \n", 6);

      curChildPID = -2;
    }
  }

}

int main(int argc, char **argv){

  if (argc < 2){
    printf("%s", USAGE);
    return 0;
  }

  // Set up signal handler

  if (signal(SIGINT, sigIntHandler) == SIG_ERR){
    perror("signal");
    return -1;
  }

  // Create file to be used for logging

  int flags = O_WRONLY | O_CREAT | O_APPEND | O_TRUNC;
  int mode = S_IRWXU;
  logFD = open(argv[1], flags, mode);

  if (logFD == -1){
    perror("open");
    return -1;
  }

  // Loop while commands are given, and execute commands in child processes

  char input[BUF_SIZE];
  ssize_t bytesRead;

  while(readyToExit == 0){

    logString("$ ", 2);

    bytesRead = read(STDIN_FILENO, input, BUF_SIZE - 1);
    input[bytesRead-1] = '\0';
    processCmd(input, bytesRead);

  }

  logString("Exiting logger shell\n", 21);

  // CLose log file
  if (close(logFD) == -1){
    perror("close");
    return -1;
  }

  return 0;

}
