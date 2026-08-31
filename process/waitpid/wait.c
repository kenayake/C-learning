#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child: Perform work and exit with a specific code
        printf("Child process (PID: %d) running.\n", getpid());
        sleep(10);
        exit(42); 
    } else {
        // Parent: Wait for the specific child
        int status;
        printf("Parent (PID: %d) waiting for child %d.\n", getpid(), pid);
        
        // Simulate waitpid polling at the end of a task cycle
        for (size_t i = 0; i < 4; i++)
        {
            // Perform a task
            sleep(3);
            
            // WUNTRACED and WCONTINUED are options; 0 for standard behavior
            pid_t terminated_pid = waitpid(pid, &status, WNOHANG);

            if (terminated_pid == -1) {
                perror("waitpid failed");
            }  else if (terminated_pid == 0) {
                printf("Child %d is still running\n", pid);
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
        
    }
    return 0;
}