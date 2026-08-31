#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
  char* command;
  char* filename;
} UserCommand;

char *trim_whitespace(char *str) {
    char *end;

    // 1. Trim leading space
    while (isspace((unsigned char)*str)) {
        str++;
    }

    // If the string is entirely whitespace
    if (*str == '\0') {
        return str;
    }

    // 2. Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    // 3. Write new null terminator
    *(end + 1) = '\0';

    return str;
}

void format_cmd(UserCommand* output, char *string) {
  output->command = trim_whitespace(strtok(string, ">"));
  output->filename = trim_whitespace(strtok(NULL, ">"));
}

int get_argc(char* string) {
  char *str_copy = strdup(string);
  if (str_copy == NULL) {
      return -1; // Memory allocation failed
  }

  int counter = 0;
  char *token = strtok(str_copy, " ");
  while (token != NULL) {
    counter++;
    
    // Pass NULL to continue splitting the same string
    token = strtok(NULL, " ");
  }
  
  // Free the allocated memory for the copy
  free(str_copy); 
  return counter;
}

int main(int argc, char** argv, char** envp) {
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    exit(1);
  }

  if (pid == 0) {
    // We in the child process
    printf("Input command: ");

    // Get input from command line
    char input[50];
    fgets(input, 50, stdin);

    // Format the input into the command and file
    UserCommand output;
    format_cmd(&output, input);

    // Open the file from user input
    int fd = open(output.filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
      perror("open failed");
      _exit(1);
    }

    // Redirect standard output to the opened file
    if (dup2(fd, STDOUT_FILENO) < 0) {
      perror("dup2 failed");
      _exit(1);
    }
    close(fd);

    // Separate user command into base command and its args
    int argc = get_argc(output.command);
    char* argv[argc + 1];
    argv[0] = strtok(output.command, " ");
    for (int i = 1; i < argc; i++) {
      argv[i] = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    // format path
    int path_size = sizeof("/bin/") + strlen(argv[0]);
    char path[path_size];
    snprintf(path, path_size, "/bin/%s", argv[0]);

    execve(path, argv, envp);

    perror("execve failed");
    _exit(1);
  } else {
    // We in the main process
    int status;
    waitpid(pid, &status, 0);
    printf("Child finished. Check output.txt\n");
  }

  return 0;
}