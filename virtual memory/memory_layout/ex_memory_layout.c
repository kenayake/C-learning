#include <stdio.h>
#include <stdlib.h>

int global_initialized = 42;
int global_uninitialized;

void function_target(void) {
    int stack_var = 10;
    printf("Stack variable address:        %p\n", (void *)&stack_var);
}

int main(int argc, char *argv[]) {
    int *heap_var = malloc(sizeof(int));
    
    printf("Text (code) segment address:   %p\n", (void *)&main);
    printf("Initialized data address:      %p\n", (void *)&global_initialized);
    printf("Uninitialized data address:    %p\n", (void *)&global_uninitialized);
    printf("Heap allocated address:        %p\n", (void *)heap_var);
    
    function_target();
    
    free(heap_var);
    return 0;
}