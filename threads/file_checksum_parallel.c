#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int file_id;
    int size;
    int checksum;
} FileTask;

void *calculate_checksum(void *arg)
{
    FileTask *task = (FileTask *)arg;

    task->checksum = (task->size * 2654435761) % 100000;

    return task;
}

int main(void)
{
    int sizes[] = {1200, 4500, 800, 3200};
    int count = sizeof(sizes) / sizeof(sizes[0]);

    pthread_t threads[count];
    FileTask tasks[count];

    // Create thread
    for (int i = 0; i < count; i++) {
        tasks[i].file_id = i;
        tasks[i].size = sizes[i];

        pthread_create(&threads[i], NULL, &calculate_checksum, (void*) &tasks[i]);
    }

    // wait for thread completion
    for (int i = 0; i < count; i++) {
        FileTask* task = &tasks[i];
        pthread_join(threads[i], (void**) &task);
    }

    // Print all processed Tasks
    for (int i = 0; i < count; i++)
    {
        printf("File %d: checksum = %d\n", tasks[i].file_id, tasks[i].checksum);
    }

    return 0;
}