#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;
    int size;
    int capacity;
} DynamicArray;

//ex1
int pop_last(DynamicArray *arr) {
    arr->size--;
    int lastItem = arr->data[arr->size];

    if (arr->size < 0.25 * arr->capacity) {
        int new_capacity = arr->capacity / 2;
        int *new_data = realloc(arr->data, new_capacity * sizeof(int));

        if (!new_data) exit(1); // Handle allocation failure

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    return lastItem;
}

// ex2
void insert_at(DynamicArray *arr, int index, int value) {
    if (index > 0 && index < arr->size) {

        if (arr->size+1 > arr->capacity) {
            int new_capacity = arr->capacity * 2;
            int *new_data = realloc(arr->data, new_capacity * sizeof(int));

            if (!new_data) exit(1);

            arr->data = new_data;
            arr->capacity = new_capacity;
        }

        for (int i = arr->size; i >= index; i--) {
            arr->data[i] = arr->data[i-1];
        }
        arr->size++;
        arr->data[index] = value;
        return;
    } 
    
    exit(1);
}

void initialize(DynamicArray *arr, int size) {
    arr->size=0;
    arr->data = malloc(size * sizeof(int));
    arr->capacity = size;
}

void push(DynamicArray *arr, int value) {
    if (arr->size == arr->capacity) {
        // Double the capacity if full
        int new_capacity = (arr->capacity == 0) ? 1 : arr->capacity * 2;
        int *new_data = realloc(arr->data, new_capacity * sizeof(int));
        
        if (!new_data) exit(1); // Handle allocation failure
        
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    arr->data[arr->size++] = value;
}

void print_arr(DynamicArray *arr){
    printf("( ");
    for (size_t i = 0; i < arr->size; i++)
    {
        printf("%d ", arr->data[i]);
    }
    printf(")\n");
}

int main(int argc, char const *argv[])
{
    DynamicArray arr;

    initialize(&arr, 0);

    for (size_t i = 0; i < 4; i++)
    {
        push(&arr, i+1);
    }
    
    int last = pop_last(&arr);

    printf("%d\n", last);
    print_arr(&arr);

    insert_at(&arr, 1, 40);
    print_arr(&arr);
    insert_at(&arr, 3, 40);
    print_arr(&arr);

    return 0;
}
