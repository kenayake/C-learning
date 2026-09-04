#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int timeout_ms;
    int max_connections;
    pthread_rwlock_t lock;
} ConfigStore;

void config_init(ConfigStore *store) {
    store->timeout_ms = 1000;
    store->max_connections = 100;
    pthread_rwlock_init(&store->lock, NULL);
}

void config_destroy(ConfigStore *store) {
    pthread_rwlock_destroy(&store->lock);
}

int config_get_timeout(ConfigStore *store) {
    pthread_rwlock_rdlock(&store->lock);
    int val = store->timeout_ms;
    pthread_rwlock_unlock(&store->lock);
    return val;
}

void config_update(ConfigStore *store, int new_timeout, int new_max) {
    pthread_rwlock_wrlock(&store->lock);
    store->timeout_ms = new_timeout;
    store->max_connections = new_max;
    printf("Configuration updated: timeout=%d, max_conn=%d\n", new_timeout, new_max);
    pthread_rwlock_unlock(&store->lock);
}

void* worker_thread(void *arg) {
    ConfigStore *store = (ConfigStore *)arg;
    for (int i = 0; i < 3; i++) {
        int timeout = config_get_timeout(store);
        printf("Worker read timeout: %d\n", timeout);
        sleep(1);
    }
    return NULL;
}

int main(void) {
    ConfigStore store;
    config_init(&store);

    pthread_t readers[4];
    for (int i = 0; i < 4; i++) {
        pthread_create(&readers[i], NULL, worker_thread, &store);
    }

    sleep(1);
    config_update(&store, 2500, 500);

    for (int i = 0; i < 4; i++) {
        pthread_join(readers[i], NULL);
    }

    config_destroy(&store);
    return 0;
}