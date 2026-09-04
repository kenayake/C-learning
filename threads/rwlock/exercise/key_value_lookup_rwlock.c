/*
 * Base: AI generated naive multithreaded key-value store backed by a pthread_mutex_t.
 *
 * Goals:
 * - change primitive mutex to rwlock
 * - measure throughput of both methods and compare
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>

#define TABLE_SIZE 64
#define NUM_READERS  4
#define NUM_WRITERS 4
#define OPS_PER_THREAD 90
#define WRITER_OPS 10

// Helper function
void microsleep(long microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;           // Whole seconds
    ts.tv_nsec = (microseconds % 1000000) * 1000; // Remaining nanoseconds

    nanosleep(&ts, NULL);
}

/* Track throughput delta */
// Simple structure to hold measurement state
typedef struct {
    atomic_size_t total_ops;
    double timestamp_sec;
} throughput_metric_t;

// Get current time in seconds with high precision
double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_nsec / 1e6);
}

// Calculate throughput delta (in bytes per second)
double calculate_throughput_delta(throughput_metric_t *tracker, double current_time) {
    size_t ops_delta = atomic_load(&tracker->total_ops);
    double time_delta = current_time - tracker->timestamp_sec;
    
    if (time_delta <= 0.0) return 0.0;
    
    return (double) ops_delta / time_delta;
}

// Init and destroy tracker
void init_tracker(throughput_metric_t *tracker) {
  tracker->total_ops = 0;
  tracker->timestamp_sec = get_time_ms();
  atomic_store(&tracker->total_ops, 0);
}

throughput_metric_t tracker;

typedef struct kv_node {
    char *key;
    char *value;
    struct kv_node *next;
} kv_node_t;

typedef struct {
    kv_node_t *buckets[TABLE_SIZE];
    pthread_rwlock_t lock;   /* single mutex protecting the whole table */
} kv_store_t;


/* --- hashing --- */
static unsigned long hash_key(const char *key) {
    unsigned long h = 5381;
    int c;
    while ((c = *key++))
        h = ((h << 5) + h) + (unsigned long)c; /* h * 33 + c */
    return h % TABLE_SIZE;
}

/* --- put: insert or update --- */
void kv_put(kv_store_t *store, const char *key, const char *value) {
    unsigned long idx = hash_key(key);

    pthread_rwlock_wrlock(&store->lock);

    kv_node_t *node = store->buckets[idx];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            free(node->value);
            node->value = strdup(value);
            pthread_rwlock_unlock(&store->lock);
            return;
        }
        node = node->next;
    }

    /* not found: insert new node at head of bucket */
    /* NOTE: unused in this exercise */
    kv_node_t *new_node = malloc(sizeof(kv_node_t));
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->next = store->buckets[idx];
    store->buckets[idx] = new_node;

    pthread_rwlock_unlock(&store->lock);
}

/* --- init / destroy --- */
void kv_init(kv_store_t *store) {
    memset(store->buckets, 0, sizeof(store->buckets));
    pthread_rwlock_init(&store->lock, NULL);

    for (int i = 0; i < 90; i++) {
      char key[32], value[32];
      int rdval = rand() % 500 + 1;
      
      snprintf(key, sizeof(key), "key-%d", i);
      snprintf(value, sizeof(value), "value-%d", rdval);

      kv_put(store, key, value);
    }
}

void kv_destroy(kv_store_t *store) {
    pthread_rwlock_wrlock(&store->lock);
    for (int i = 0; i < TABLE_SIZE; i++) {
        kv_node_t *node = store->buckets[i];
        while (node) {
            kv_node_t *next = node->next;
            free(node->key);
            free(node->value);
            free(node);
            node = next;
        }
        store->buckets[i] = NULL;
    }
    pthread_rwlock_unlock(&store->lock);
    pthread_rwlock_destroy(&store->lock);
}

/*
 * get: returns 1 and copies the value into out_value (caller-provided
 * buffer of size out_size) if found, otherwise returns 0.
 */
int kv_get(kv_store_t *store, const char *key, char *out_value, size_t out_size) {
    unsigned long idx = hash_key(key);
    int found = 0;

    pthread_rwlock_rdlock(&store->lock);

    kv_node_t *node = store->buckets[idx];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            snprintf(out_value, out_size, "%s", node->value);
            found = 1;
            break;
        }
        node = node->next;
    }

    pthread_rwlock_unlock(&store->lock);
    return found;
}

typedef struct {
    kv_store_t *store;
    int reader_id;
} reader_args_t;

void *reader(void *arg) {
    microsleep(1000);
    reader_args_t *args = (reader_args_t *)arg;
    char key[32], out[32];

    for (int i = 0; i < OPS_PER_THREAD; i++) {
        int rdkey = rand() % 90;
        snprintf(key, sizeof(key), "key-%d", rdkey);

        if (kv_get(args->store, key, out, sizeof(out))) {
            // Add the operation to the tracker
            atomic_fetch_add(&tracker.total_ops, 1); 
            printf("[thread %d] %s = %s\n", args->reader_id, key, out);
        }
    }
    return NULL;
}

void *writer(void *args) {
  kv_store_t *store = (kv_store_t *)args;
  char key[32], value[32];

  for (int i = 0; i < WRITER_OPS; i++) {
    microsleep(1000);
    int rdkey = rand() % 90;
    int rdval = rand() % 500 + 500;
    snprintf(key, sizeof(key), "key-%d", rdkey);
    snprintf(value, sizeof(value), "value-%d", rdval);

    kv_put(store, key, value);
    // Add the operation to the tracker
    atomic_fetch_add(&tracker.total_ops, 1); 
  }
    return NULL;
}

int main(void) {
    // Init throughtput tracker
    init_tracker(&tracker);

    kv_store_t store;
    kv_init(&store);

    pthread_t readers[NUM_READERS];
    reader_args_t args[NUM_READERS];

    pthread_t writers[NUM_WRITERS];


    for (int i = 0; i < NUM_READERS; i++) {
        args[i].store = &store;
        args[i].reader_id = i;
        pthread_create(&readers[i], NULL, reader, &args[i]);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
      pthread_create(&writers[i], NULL, writer, &store);
    }

    for (int i = 0; i < NUM_READERS; i++)
        pthread_join(readers[i], NULL);

    for (int i = 0; i < NUM_WRITERS; i++)
        pthread_join(writers[i], NULL);


    // Measure throughput delta and report
    double now = get_time_ms();
    double throughput_delta = calculate_throughput_delta(&tracker, now);
    double time_taken = (now - tracker.timestamp_sec);
    size_t ops = atomic_load(&tracker.total_ops);
    printf("Ran %zu operations for %.6fms Throughput: %.2f ops/ms\n", ops, time_taken, throughput_delta);
    
    kv_destroy(&store);
    return 0;
}