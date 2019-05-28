#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#define STACKSIZE (1024 * 1024)

struct params{

  char * cmd;
  char ** argv;
  int cpu_pct;
  int mem_limit;
  int num_levels;

};


static char stack[STACKSIZE];
void print_err(char const * const reason)
{
    fprintf(stderr, "Error %s: %s\n", reason, strerror(errno));
}
int exec(void * args)
{
    // Remount proc
    // Use print_err to print error if mount fails
  // int mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);

  int result;
  result = mount("proc", "/proc", "proc", 0, NULL);
  if (result != 0){
    print_err("mount");
  }

    char const * const hostname = "CSC595";
    // Set a new hostname
    // Use print_err to print error if sethostname fails
    result = sethostname(hostname, strlen(hostname));

    if (result != 0){
      print_err("sethostname");
    }

    // Create a message queue
    // USe print_err to print error if creating message queue fails
    result = msgget(0, IPC_CREAT | 0666);
    if (result < 0){
      print_err("msgget");
    }


    char ** const argv = args;
    // Execute the given command
    // Use print_err to print error if execvp fails
    execvp(argv[0], argv);


    return 0;
}
int main(int argc, char ** argv)
{
    // Provide some feedback about the usage
    if (argc < 2) {
        fprintf(stderr, "No command specified\n");
        return 1;
    }


    // Initialize params struct
    struct params userParams;
    userParams.cmd = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(userParams.cmd, argv[1]);

    userParams.cpu_pct = atoi(argv[2]);
    userParams.mem_limit = atoi(argv[3]);
    userParams.num_levels = atoi(argv[4]);

    // Fill in additional args
    int numArgs = argc - 5;
    if (numArgs > 0){
      userParams.argv = (char **)malloc(sizeof(char *) * numArgs);
    }

    for (int i = 5; i < argc; i++){
      userParams.argv[i-5] = (char *)malloc(strlen(argv[i]) + 1);
      strcpy(userParams.argv[i - 5], argv[i]);
    }

    printf("cmd: %s\n", userParams.cmd);
    printf("cpu_pct: %d\n", userParams.cpu_pct);
    printf("mem_limit: %d\n", userParams.mem_limit);
    printf("num_levels: %d\n", userParams.num_levels);

    for (int i = 0; i < numArgs; i++){
      printf("Arg #%d: %s\n", i, userParams.argv[i]);
    }

    // Namespace flags
    // ADD more flags
    const int flags = SIGCHLD | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWUSER;

    // Create a new child process
    pid_t pid = clone(exec, stack + STACKSIZE, flags, &argv[1]);
    if (pid < 0) {
        print_err("calling clone");
        return 1;
    }

    // Wait for the process to finish
    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        print_err("waiting for pid");
        return 1;
    }
    // Return the exit code
    return WEXITSTATUS(status);
}
