#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  int fd1 = open("hw2f1", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

  if (fd1 == -1){
    perror("open");
    return -1;
  }

  int fd2 = dup(fd1);

  if (fd2 == -1){
    perror("open");
    return -1;
  }

  int fd3 = open("hw2f1", O_RDWR);

  if (fd3 == -1){
    perror("open");
    return -1;
  }

  int fd4 = open("hw2f1", O_WRONLY | O_APPEND);

  if (fd4 == -1){
    perror("open");
    return -1;
  }



  ssize_t bytesWritten;

  if ((bytesWritten = write(fd1, "Hello,", 6)) < 6){
    perror("write");
    return -1;
  }

  if ((bytesWritten = write(fd2, " world", 6)) < 6){
    perror("write");
    return -1;
  }

  off_t err;
  if ((lseek(fd2, 0, SEEK_SET)) == -1){
    perror("lseek");
    return -1;
  }


  if ((bytesWritten = write(fd4, " Hej da!", 8)) < 6){
    perror("write");
    return -1;
  }

  if ((bytesWritten = write(fd1, "HELLO,", 6)) < 6){
    perror("write");
    return -1;
  }

  if ((bytesWritten = write(fd3, "Gidday", 6)) < 6){
    perror("write");
    return -1;
  }

  if ((err = lseek(fd4, 0, SEEK_SET)) == -1){
    perror("lseek");
    return -1;
  }

  if ((bytesWritten = write(fd4, " Var ar vi?", 11)) < 6){
    perror("write");
    return -1;
  }

}
