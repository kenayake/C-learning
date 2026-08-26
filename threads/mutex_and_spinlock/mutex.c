#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 4

typedef struct {
    int shared_counter;
    pthread_mutex_t  lock;
} shared_data_t;

void* increment_counter(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;

    for (int i = 0; i < 100000; ++i) {
        // Acquire lock: if held by another thread, this thread sleeps
        pthread_mutex_lock(&data->lock);
        
        // Critical Section
        data->shared_counter++;
        
        // Release lock: kernel wakes one waiting thread
        pthread_mutex_unlock(&data->lock);
    }
    return NULL;
}

void* fail_func(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;

    int result = 0;

    for (int i = 0; i < 5000; i++) {
        result = pthread_mutex_unlock(&data->lock);
    }

    printf("Unlock Result: %d\n", result);

    return NULL;
}

int main() {
    shared_data_t data = { .shared_counter = 0, .lock = PTHREAD_MUTEX_INITIALIZER };
    pthread_t threads[NUM_THREADS];
    pthread_t failth;

    // Setup internal timer
    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

    // Create and join threads
    for (size_t i = 0; i < NUM_THREADS; i++)
    {
      pthread_create(&threads[i], NULL, increment_counter, &data);
    }
    pthread_create(&failth, NULL, fail_func, &data);

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
    }
    pthread_join(failth, NULL);

    // Record execution time
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    printf("Final counter value: %d\n", data.shared_counter);

    printf ("Total time = %f seconds\n",
            (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
            (end.tv_sec  - begin.tv_sec));

    pthread_mutex_destroy(&data.lock);
    return 0;
}