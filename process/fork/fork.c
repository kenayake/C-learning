#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int counter = 0;

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // We are in the child process
        printf("Child process: PID is %d\n", getpid());
        printf("Parent process: PID is %d\n", getppid());

        counter++;
        sleep(1);
    } else {
        // We are in the parent process
        printf("Parent process: PID is %d\n", getpid());
        printf("Parent Parent process: PID is %d\n", getppid());

        counter++;
        sleep(1);
    }

    printf("counter: %d\n", counter);
    return 0;
}