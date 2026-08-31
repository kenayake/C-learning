#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid[3];

  for (size_t i = 0; i < 3; i++)
  {
    pid[i] = fork();
    
    if (pid[i] < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid[i] == 0) {
          // Child: Perform work and exit with a specific code
          printf("Child process (PID: %d) running.\n", getpid());
          sleep((rand() % 5) + 1);
          exit(42); 
    }
  }

  int status;

  pid_t terminated_pid = waitpid(-1, &status, 0);

  if (terminated_pid == -1) {
      perror("waitpid failed");
  }
  else {
      // Use macros to interpret the status integer
      if (WIFEXITED(status)) {
          printf("Child %d exited with status: %d\n", 
              terminated_pid, WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
          printf("Child %d killed by signal: %d\n", 
              terminated_pid, WTERMSIG(status));
      }
  }
}