#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 4
#define TRANSACTIONS_AMOUNT 2168

typedef enum { WITHDRAW, DEPOSIT } TransactionType;

typedef struct {
  int amount;
  TransactionType type;
} Transaction;

int account_balance = 1000;

// Test data
void create_transactions(Transaction *transactions) {
  for (int i = 0; i < TRANSACTIONS_AMOUNT; i++) {
    transactions[i].amount = rand() % 500 + 1;
    transactions[i].type = rand() % 2;
  }
}

// Init mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *routine(void *arg) {

  // printf("Thread %d started\n", *(int *)arg);

  // Init file reader
  FILE *fptr;

  // goofy ahh string formatting
  char filename[64];
  int index = *(int *)arg;
  snprintf(filename, sizeof(filename), "threads/transactions%d.txt", index);
  fptr = fopen(filename, "r");
  if (fptr == NULL) {
    perror("Error opening file");
    exit(1);
  }
  char trxString[20];

  // Read file per line
  while (fgets(trxString, 20, fptr)) {

    // Get transaction type and amount from line
    char *token = strtok(trxString, " ");

    if (token == NULL) {
      printf("%s\n", trxString);
      continue;
    };
    char *transaction_type = token;
    token = strtok(NULL, " ");
    if (token == NULL) {
      printf("%s\n", trxString);
      continue;
    };
    int amount = (int)strtol(token, NULL, 10);

    // lock resources for thread
    pthread_mutex_lock(&mutex);
    // sleep(1);
    if (strcmp(transaction_type, "withdraw") == 0) {

      if (account_balance < amount) {
        printf("Withdrawal of $%d Rejected\n", amount);
      } else {
        account_balance -= amount;
      }

    } else {

      account_balance += amount;
    }

    //free and unlock
    pthread_mutex_unlock(&mutex);
  }

  fclose(fptr); 
  // printf("Thread %d ended\n", *(int *)arg);
  return NULL;
}

int main() {

  // Bank account balance
  printf("Initial balance %d\n", account_balance);

  // initialize threads
  pthread_t users[NUM_THREADS];

  for (int run = 0; run < 100; run++)
  {
    int indices[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
      indices[i] = i + 1;
      if (pthread_create(&users[i], NULL, routine, &indices[i]) != 0) {
        printf("Thread %d creation failed\n", i);
        return 1;
      }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
      if (pthread_join(users[i], NULL) != 0) {
        printf("Thread %d creation failed\n", i);
        return 1;
      }
    }

    printf("Final balance: %d\n", account_balance);
    account_balance = 1000;
  }

  return 0;
}