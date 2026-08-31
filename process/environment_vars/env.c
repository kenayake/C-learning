#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[], char *envp[]) {
    // envp and environ point to the same null-terminated array of strings
    // for (int i = 0; environ[i] != NULL; i++) {
    //     printf("%s\n", environ[i]);
    // }

    if (argc > 1)
    {
        char* env_val = getenv(argv[1]);
        if (env_val == NULL)
        {
            printf("Cannot find env/n");
        } else {
            printf("Got env %s with value %s\n", argv[1]);
        }
    }
    return 0;
}