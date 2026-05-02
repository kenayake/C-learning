#include <stdio.h>
#include <stdlib.h>

int *returnLocal() {
    int baba = 12345;

    return &baba;
}

int *returnHeap() {
    int *asa = malloc(sizeof *asa);

    if (asa == NULL) {
        return NULL;
    }

    *asa = 12345;

    return asa;
}

int main(int argc, char const *argv[])
{
    int *localvar = returnLocal();
    int *heapvar = returnHeap();

    printf("%d\n", *localvar);
    printf("%d\n", *heapvar);
    free(heapvar);
    return 0;
}
