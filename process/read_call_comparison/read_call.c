#include <bits/time.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


int fd1;
int fd2;
char buffer1[2];
char buffer2[10001];


void many_read() {
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < 10000; i++) {
    read(fd2, buffer1, sizeof(buffer1) - 1);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("Read 1 byte 10000 times: %.10f seconds\n", elapsed_time);

}

void read_many() {
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  read(fd2, buffer2, sizeof(buffer2) - 1);
  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("Read 10000 bytes 1 times: %.10f seconds\n", elapsed_time);
}

int main() {

  fd1 = open("one.txt", O_RDONLY);
  fd2 = open("many.txt", O_RDONLY);

  read_many();
  many_read();

  return 0;
}