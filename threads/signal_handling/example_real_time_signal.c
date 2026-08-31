#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WORKER_SIGNAL (SIGRTMIN + 1)

static void *worker_thread(void *arg) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, WORKER_SIGNAL);

    printf("[Worker] Thread started, waiting for real-time signal payload...\n");

    while (1) {
        siginfo_t info;
        // Wait for real-time signal and retrieve accompanying payload data
        if (sigwaitinfo(&set, &info) < 0) {
            perror("sigwaitinfo failed");
            continue;
        }

        if (info.si_signo == WORKER_SIGNAL) {
            // Access payload sent via si_value
            int task_id = info.si_value.sival_int;
            printf("[Worker] Received task ID %d from sender PID %d\n", 
                   task_id, info.si_pid);
            
            if (task_id == -1) {
                printf("[Worker] Termination signal received. Exiting.\n");
                break;
            }
        }
    }
    return NULL;
}

int main(void) {
    pthread_t worker;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, WORKER_SIGNAL);
    
    // Block the real-time signal in the main thread so worker captures it
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_create(&worker, NULL, worker_thread, NULL);
    sleep(1); // Allow worker to initialize

    // Send queued signals with integer payloads from the same process
    union sigval value;
    for (int i = 1; i <= 3; i++) {
        value.sival_int = i * 100;
        printf("[Main] Queuing real-time signal with payload: %d\n", value.sival_int);
        if (sigqueue(getpid(), WORKER_SIGNAL, value) < 0) {
            perror("sigqueue failed");
        }
    }

    // Send termination payload
    value.sival_int = -1;
    sigqueue(getpid(), WORKER_SIGNAL, value);

    pthread_join(worker, NULL);
    return 0;
}