#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int x = 7;
int y;
char *q = "Some words";
char s[1000000 * 10];

int main(int argc, char *argv[]) {

  pid_t pid = fork();

  if (pid) {
    x = 11;
    y = 42;
  }
  else {
    y = 999;
    q = "What are x, y, and q?";
    static char *q2 = "Where is this?";
  }
  printf("%d: %d %d %s\n", pid, x, y, q);
  return 0;
}
