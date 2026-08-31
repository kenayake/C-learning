#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void *signal_handler_thread(void *arg) {
    sigset_t *set = (sigset_t *)arg;
    int sig;

    while (1) {
        // Synchronously wait for a signal from the set
        int s = sigwait(set, &sig);
        if (s != 0) {
            perror("sigwait failed");
            break;
        }

        switch (sig) {
            case SIGINT:
                printf("\n[Signal Thread] Caught SIGINT (Ctrl+C). Cleaning up safely...\n");
                // Perform safe cleanup here (mutexes, state flushing, etc.)
                exit(EXIT_SUCCESS);
                break;
            case SIGTERM:
                printf("\n[Signal Thread] Caught SIGTERM. Shutting down...\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("\n[Signal Thread] Caught unexpected signal %d\n", sig);
                break;
        }
    }
    return NULL;
}

int main(void) {
    pthread_t thread_id;
    sigset_t set;

    // 1. Initialize signal set and add signals we want to manage
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);

    // 2. Block signals in the main thread. Any subsequently created threads 
    // will inherit this signal mask.
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask failed");
        exit(EXIT_FAILURE);
    }

    // 3. Spawn the dedicated signal handling thread
    if (pthread_create(&thread_id, NULL, signal_handler_thread, &set) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    // 4. Main application workload continues uninterrupted by signals
    printf("Application running. PID: %d. Press Ctrl+C to trigger SIGINT.\n", getpid());
    
    while (1) {
        sleep(1);
    }

    pthread_join(thread_id, NULL);
    return 0;
}