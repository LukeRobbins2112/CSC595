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
#define STACKSIZE (1024 * 1024)


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
    if (result != 0){
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
