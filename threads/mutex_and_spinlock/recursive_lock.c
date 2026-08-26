#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int shared_counter;
    pthread_mutex_t lock;
} shared_data_t;

void* increment_counter(void* arg) {
    shared_data_t* data = (shared_data_t*)arg;

    for (int i = 0; i < 100000; ++i) {
        // Acquire lock: if held by another thread, this thread sleeps
        pthread_mutex_lock(&data->lock);
        pthread_mutex_lock(&data->lock);
        
        // Critical Section
        data->shared_counter++;
        
        // Release lock: kernel wakes one waiting thread
        pthread_mutex_unlock(&data->lock);
        pthread_mutex_unlock(&data->lock);
    }
    return NULL;
}

int main() {
    shared_data_t data = { .shared_counter = 0 };
    pthread_mutexattr_t mutex_attr;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&data.lock, &mutex_attr);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment_counter, &data);
    pthread_create(&t2, NULL, increment_counter, &data);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", data.shared_counter);

    pthread_mutexattr_destroy(&mutex_attr);
    pthread_mutex_destroy(&data.lock);
    return 0;
}