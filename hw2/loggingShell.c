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
int curChildPID = -1;

//----------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------

void logString(const char * msg, size_t bytes);

//----------------------------------------------------------------
// Functions
//----------------------------------------------------------------

static void sigIntHandler(int sig){

  if (curChildPID == -1){
    logString("No child process executing\n", 27);
    return;
  }

  char childPIDString[5];
  // itoa(curChildPID, childPIDString, 10);

  // Send termination signal to child process
  logString("Terminating child process ", 26);
  // logString(childPIDString, 5);
  logString("\n", 1);
  kill(curChildPID, SIGINT);

}

void logString(const char * msg, size_t bytes){

  write(logFD, msg, bytes);
  write(STDOUT_FILENO, msg, bytes);

}

void processCmd(const char * cmd, ssize_t len){

  logString("Processing command: ", 20);
  logString(cmd, len);
  logString("\n", 1);


  if (strcmp(cmd, "exit") == 0){
    readyToExit = 1;
  }
  else{

    // Spawn a new process
    curChildPID = fork();

    if (curChildPID == -1){
      logString("Could not run program", 21);
    }
    else if (curChildPID == 0){
      int res = execl(cmd, cmd, NULL);

      if (res == -1){
        logString("exec failed\n", 12);
        _exit(-1);
      }
    }
    else{
      logString("\n+++ Child [childPID] executes ", 31);
      logString(cmd, len);
      logString("\n", 1);

      int status;
      waitpid(curChildPID, &status, 0);
      logString("Child [childPID] finished executing", 35);
      logString(cmd, len);
      logString("\n", 1);
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

  int flags = O_WRONLY | O_CREAT | O_APPEND;
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

  // CLose log file
  if (close(logFD) == -1){
    perror("close");
    return -1;
  }

  return 0;

}
