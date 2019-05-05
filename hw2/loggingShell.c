#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const char * USAGE = "USAGE loggingShell [logfile]\n";

int main(int argc, char **argv){

  if (argc < 2){
    printf("%s", USAGE);
    return 0;
  }



}
