#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main(int argc, char *argv[], char *envp[]) {
  if (setenv("SECRET_KEY", "SOMEBODY TOUCHA MA SPAGHETT", 1) != 0) {
    perror("setenv failed");
    exit(1);
  }
  
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed");
    exit(1);
  }

  // We in the child process now
  if (pid == 0) {
    char* args[2] = { "env", "SECRET_KEY" };

    execve("process/environment_vars/env.exe", args, environ);

    // If execve returns, an error occurred
    perror("execve failed");
    _exit(1);
  } else {
      // Parent waits for the child to complete
      int status;
      waitpid(pid, &status, 0);
  }

  return 0;
}