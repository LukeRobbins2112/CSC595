#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const char * USAGE = "USAGE loggingShell [logfile]\n";

int logFD;

void logString(const char * msg, size_t bytes){

  write(logFD, msg, bytes);
  write(STDOUT_FILENO, msg, bytes);

}

int main(int argc, char **argv){

  if (argc < 2){
    printf("%s", USAGE);
    return 0;
  }

  // Create file to be used for logging

  int flags = O_WRONLY | O_CREAT | O_APPEND;
  int mode = S_IRWXU;
  logFD = open(argv[1], flags, mode);

  if (logFD == -1){
    perror("open");
    return -1;
  }

  logString("test log\n", 9);

  return 0;

}
