#include <stddef.h>
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define ARRAY_SIZE 1000000

typedef struct {
    int *array;
    int start;      // inclusive
    int end;        // exclusive
    long long partial_sum;
} ThreadData;

void *sum_worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long long sum = 0;
    for (int i = data->start; i < data->end; i++) {
        sum += data->array[i];
    }
    data->partial_sum = sum;
    return NULL;
}

int main(void) {
    int *array = malloc(sizeof(int) * ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1; // 1..1,000,000
    }

    struct timespec start, end;

    // Record start time using the non-adjustable monotonic clock
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int chunk = ARRAY_SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].array = array;
        thread_data[i].start = i * chunk;
        thread_data[i].end = (i == NUM_THREADS - 1) ? ARRAY_SIZE : (i + 1) * chunk;

        // TODO: create the thread, running sum_worker on &thread_data[i]
        pthread_create(&threads[i], NULL, sum_worker, &thread_data[i]);
    }

    long long total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        // TODO: join the thread before reading its result
        pthread_join(threads[i], NULL);
        total += thread_data[i].partial_sum;
    }

    // Single threaded
    // for (size_t i = 0; i < ARRAY_SIZE; i++)
    // {
    //   total += array[i];
    // }

    // Record end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    printf("Total sum: %lld\n", total);
    free(array);

    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    printf("Elapsed time: %.6f seconds\n", elapsed);
    return 0;
}