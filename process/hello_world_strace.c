#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  printf("Hello world\n");

  // Purposely fail the write
  int fail = write(STDOUT_FILENO, (void *)0x1, 1);

  printf("%s\n", strerror(errno));

  return 0;
}