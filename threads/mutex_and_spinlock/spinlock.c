#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_THREADS 4

typedef struct {
    int shared_counter;
    pthread_spinlock_t  lock;
} shared_data_t;

void* increment_counter(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;

    for (int i = 0; i < 100000; ++i) {
        // Acquire lock: if held by another thread, this thread sleeps
        pthread_spin_lock(&data->lock);
        
        // Critical Section
        data->shared_counter++;
        
        // Release lock: kernel wakes one waiting thread
        pthread_spin_unlock(&data->lock);
    }
    return NULL;
}

int main() {
    shared_data_t data = { .shared_counter = 0 };
    pthread_spin_init(&data.lock, PTHREAD_PROCESS_PRIVATE);
    pthread_t threads[NUM_THREADS];

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
      pthread_create(&threads[i], NULL, increment_counter, &data);
    }

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    printf("Final counter value: %d\n", data.shared_counter);

    printf ("Total time = %f seconds\n",
            (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
            (end.tv_sec  - begin.tv_sec));

    pthread_spin_destroy(&data.lock);
    return 0;
}