#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 4

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int data_ready;
} task_queue;

void* producer(void* arg) {
  task_queue* q = (task_queue*)arg;
  
  for (int i = 0; i < 3; i++) {
    sleep(1); // Simulate work

    pthread_mutex_lock(&q->mutex);
    q->data_ready = 1;
    printf("Producer: Signaling that data is ready.\n");
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
  }  
    
    return NULL;
}

void* consumer(void* arg) {
    task_queue* q = (task_queue*)arg;

    pthread_mutex_lock(&q->mutex);
    while (!q->data_ready) {
        printf("Consumer: Waiting for data...\n");
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    printf("Consumer: Data received, processing.\n");
    q->data_ready = 0;
    pthread_mutex_unlock(&q->mutex);
    return NULL;
}

int main() {
  pthread_t producer_th, consumer_th[NUM_THREADS];
  task_queue task;

  // Initialize mutex and conditional variable
  pthread_mutex_init(&task.mutex, NULL);
  pthread_cond_init(&task.cond, NULL);

  // Create producer thread
  if (pthread_create(&producer_th, NULL, producer, &task) != 0) {
    printf("Producer creation failed\n");
    return 1;
  }

  // Create consumer thread
  for (int i = 0; i < NUM_THREADS; i++)
  {
    if (pthread_create(&consumer_th[i], NULL, consumer, &task)) {
      printf("Consumer %d creation failed\n", i);
      return 1;
    }
  }

  // Join threads and destroy mutexes
  if (pthread_join(producer_th, NULL) != 0) {
    printf("Producer join failed\n");
    return 1;
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    if (pthread_join(consumer_th[i], NULL)) {
      printf("Consumer %d creation failed\n", i);
      return 1;
    }
  }

  pthread_mutex_destroy(&task.mutex);
  pthread_cond_destroy(&task.cond);

  return 0;
}