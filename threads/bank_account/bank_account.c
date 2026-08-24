#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 10
#define NUM_OPS 100000

typedef struct {
  double balance;
  pthread_mutex_t lock; // TODO: initialize this in main, use it in the worker
} Account;

typedef struct {
  Account *account;
  int thread_id;
} WorkerArgs;

void *transaction_worker(void *arg) {
  WorkerArgs *args = (WorkerArgs *)arg;
  Account *acc = args->account;

  for (int i = 0; i < NUM_OPS; i++) {
    pthread_mutex_lock(&acc->lock);

    acc->balance += 1.0; // deposit $1
    acc->balance -= 1.0; // withdraw $1
    
    pthread_mutex_unlock(&acc->lock);
  }
  return NULL;
}

int main(void) {
  struct timespec start, end;

  // Record start time using the non-adjustable monotonic clock
  clock_gettime(CLOCK_MONOTONIC, &start);

  Account account;
  account.balance = 1000.0;
  pthread_mutex_init(&account.lock, NULL);

  pthread_t threads[NUM_THREADS];
  WorkerArgs args[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    args[i].account = &account;
    args[i].thread_id = i;
    pthread_create(&threads[i], NULL, transaction_worker, &args[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Final balance: %.2f (expected: 1000.00)\n", account.balance);

  pthread_mutex_destroy(&account.lock);

  // Record end time
  clock_gettime(CLOCK_MONOTONIC, &end);
  
  double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  
  printf("Elapsed time: %.6f seconds\n", elapsed);
  return 0;
}