#include <unistd.h>
#include <stdio.h>

int main() {
    char *args[] = {"/bin/ls", "-l", NULL};
    char *env[] = {NULL};

    printf("Executing ls...\n");
    
    // Replace the current process with /bin/ls
    int result = execve("process/imaginary_program", NULL, NULL);
    printf("execve failed with %d\n", result);

    // This line is only reached if execve fails
    perror("execve failed");
    return 1;
}