#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const char * USAGE = "USAGE loggingShell [logfile]\n";

void printString(const char * msg){


}

int main(int argc, char **argv){

  if (argc < 2){
    printf("%s", USAGE);
    return 0;
  }

  // Create file to be used for logging

  int flags = O_WRONLY | O_CREAT | O_APPEND;
  int mode = S_IRWXU;
  int logFD = open(argv[1], flags, mode);

  if (logFD == -1){
    perror("open");
    return -1;
  }

  write(logFD, "test\n", 5);

  return 0;

}
