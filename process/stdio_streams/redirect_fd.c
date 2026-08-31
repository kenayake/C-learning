#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        
        // Open a file for writing. Create it if it doesn't exist, truncate if it does.
        int fd = open("process/stdio_streams/output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open failed");
            _exit(1);
        }

        // Redirect standard output (1) to the opened file (fd)
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 failed");
            _exit(1);
        }

        // We no longer need the original fd copy; dup2 created a distinct reference
        close(fd);

        // Execute a command that writes to stdout
        char *args[] = {"echo", "Hello, redirected world!", NULL};
        execve("/bin/echo", args, __environ);
        
        perror("execve failed");
        _exit(1);
    } else {
        // Parent process waits for child
        int status;
        waitpid(pid, &status, 0);
        printf("Child finished. Check output.txt\n");
    }

    return 0;
}