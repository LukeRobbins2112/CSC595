#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {

  int fd1 = open("out.txt", O_RDWR);
  char str[] = "xyz";
  char c;
  write(fd1, str, 1);
  read(fd1, &c, 1);
  write(fd1, &c, 1);
  return 0;

}
