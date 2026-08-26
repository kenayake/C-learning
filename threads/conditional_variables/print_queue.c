#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define QUEUE_CAPACITY 5
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 2
#define JOBS_PER_PRODUCER 5

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
int remaining = 3;

void *producer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < JOBS_PER_PRODUCER; i++) {
        PrintJob job = { .job_id = i, .producer_id = id };
        printf("[Producer %d] submitting job %d\n", id, i);
        enqueue(&queue, job);

        struct timespec sleepTime = { 0, 10000000 };
        nanosleep(&sleepTime, NULL);
    }
    pthread_mutex_lock(&queue.lock);
    remaining--;
    pthread_mutex_unlock(&queue.lock);

    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    
    // Loop calling dequeue until no more remaining jobs and queue is empty
    while (remaining > 0 || queue.count != 0)
    {
        PrintJob job = dequeue(&queue);

        printf("[Consumer %d] printing job %d from producer %d\n", id, job.job_id, job.producer_id);

        struct timespec sleepTime = { 0, 10000000 };
        nanosleep(&sleepTime, NULL);
    }
    (void)id;
    return NULL;
}

int main(void) {
    queue_init(&queue);

    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t consumer_threads[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumer_threads[i], NULL, consumer, &consumer_ids[i]);
    }
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producer_threads[i], NULL, producer, &producer_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) pthread_join(producer_threads[i], NULL);

    // TODO: signal consumers that no more jobs are coming, then join them
    for (int i = 0; i < NUM_CONSUMERS; i++) pthread_join(consumer_threads[i], NULL);

    printf("All %d jobs processed.\n", NUM_PRODUCERS * JOBS_PER_PRODUCER);
    return 0;
}