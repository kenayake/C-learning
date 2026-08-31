#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void *signal_handler_thread(void *arg) {
    sigset_t* set = (sigset_t *)arg;
    int sig;

    while (1) {
        // Synchronously wait for a signal from the set
        int s = sigwait(set, &sig);
        if (s != 0) {
            perror("sigwait failed");
            return NULL;
        }

        switch (sig) {
            case SIGINT:
                printf("\n[Signal Thread] Caught SIGINT (Ctrl+C). Printing status report...\n");
                sleep(1);
                printf("Sig: %d\n", sig);
                break;
            case SIGQUIT:
                printf("\n[Signal Thread] Caught SIGQUIT. Shutting down...\n");
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

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
      perror("pthread_sigmask");
      return 1;
    }

    if (pthread_create(&thread_id, NULL, signal_handler_thread, &set) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    printf("Application running. PID: %d. Press Ctrl+C to trigger SIGINT or Ctrl+\\ to triger SIGQUIT.\n", getpid());
    
    while (1) {
        sleep(1);
    }

    pthread_join(thread_id, NULL);
    return 0;
}