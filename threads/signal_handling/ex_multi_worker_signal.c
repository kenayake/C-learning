/* Base code copied from conditional_variables/print_queue exercise */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define QUEUE_CAPACITY 5
#define NUM_PRODUCERS 3
#define NUM_WORKERS 2
#define JOBS_PER_PRODUCER 5
#define WORKER_SIGNAL (SIGRTMIN + 1)

typedef struct {
    int job_id;
    int producer_id;
} PrintJob;

typedef struct {
    PrintJob jobs[QUEUE_CAPACITY];
    int head, tail, count;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} JobQueue;

void queue_init(JobQueue *q) {
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void enqueue(JobQueue *q, PrintJob job) {
    pthread_mutex_lock(&q->lock);
    
    // wait on not_full condition if queue is full (count equals capacity)
    while (q->count == QUEUE_CAPACITY) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    // if queue is not full, add a job to the top (tail) of the stack
    // increment count and tail pointer
    q->jobs[q->tail] = job;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->count++;

    // signal 
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

PrintJob dequeue(JobQueue *q) {
    pthread_mutex_lock(&q->lock);
    PrintJob job = {0};
    
    // wait on not_empty condition if queue is empty (count is 0)
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    // if queue is not empty, take a job from the botton (head) of the stack
    // increment count and head pointer
    job = q->jobs[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return job;
}

JobQueue queue;

void *producer(void *arg) {
  int id = *(int *)arg;

  for (int i = 0; i < JOBS_PER_PRODUCER; i++) {
    PrintJob *job = malloc(sizeof(PrintJob));
    job->job_id = i;
    job->producer_id = id;
    printf("[Producer %d] submitting job %d\n", id, i);

    // Send the job as signal
    union sigval value;
    value.sival_ptr = job;
    if (sigqueue(getpid(), WORKER_SIGNAL, value) < 0) {
      perror("sigqueue failed");
    }

    struct timespec sleepTime = { 0, 10000000 };
    nanosleep(&sleepTime, NULL);
  }
}

void *worker(void *arg) {
    int id = *(int *)arg;
    
    // Loop calling dequeue until no more remaining jobs and queue is empty
    while (1)
    {
        PrintJob job = dequeue(&queue);

        printf("[Consumer %d] printing job %d from producer %d\n", id, job.job_id, job.producer_id);

        struct timespec sleepTime = { 0, 10000000 };
        nanosleep(&sleepTime, NULL);
    }
    (void)id;
    return NULL;
}

void *signal_manager() {
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, WORKER_SIGNAL);

  while (1) {
    siginfo_t info;
    // Wait for real-time signal and retrieve accompanying payload data
    if (sigwaitinfo(&set, &info) < 0) {
      perror("sigwaitinfo failed");
      continue;
    }

    if (info.si_signo == WORKER_SIGNAL) {
      // Access payload sent via si_value
      PrintJob job = *(PrintJob *) info.si_value.sival_ptr;
      printf("Forwarding job %d from producer %d to consumers\n", job.job_id, job.producer_id);
      enqueue(&queue, job);
    }
  }

  return NULL;
}

int main(void) {
    queue_init(&queue);

    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t worker_threads[NUM_WORKERS];
    // Create the signal manager thread
    pthread_t sig_manager_thread;
    int producer_ids[NUM_PRODUCERS];
    int worker_ids[NUM_WORKERS];

    // Add the worker signal to the set and block it
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, WORKER_SIGNAL);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    for (int i = 0; i < NUM_WORKERS; i++) {
        worker_ids[i] = i;
        pthread_create(&worker_threads[i], NULL, worker, &worker_ids[i]);
    }
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producer_threads[i], NULL, producer, &producer_ids[i]);
    }
    if (pthread_create(&sig_manager_thread, NULL, signal_manager, NULL)) {
      perror("failed to create signal manager thread");
      return(1);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) pthread_join(producer_threads[i], NULL);

    // TODO: signal consumers that no more jobs are coming, then join them
    for (int i = 0; i < NUM_WORKERS; i++) pthread_join(worker_threads[i], NULL);

    printf("All %d jobs processed.\n", NUM_PRODUCERS * JOBS_PER_PRODUCER);
    return 0;
}