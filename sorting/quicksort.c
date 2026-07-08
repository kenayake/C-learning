#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

void printarr(int* arr, int size, int selection, int selection2) {
    printf("{ ");
    for (int i = 0; i < size; i++) {
        if (selection != -1 && i == selection || selection2 != -1 && i == selection2) {
            printf("[%d]", arr[i]);
        } else {
            printf("%d", arr[i]);
        }
        if (i != size-1) {
            printf(", ");
        } else {
            printf(" ");
        }
    }
    printf("}\n");
}

inline unsigned long long collect_cycles() {
    unsigned int ui;
    // __rdtscp returns the 64-bit time-stamp counter
    return __rdtscp(&ui);
}

// Swap the values at two integer pointers.
void swap_ints(int* first, int* second) {
    int temp = *first; *first = *second; *second = temp;
}

// Partition the array using the last element as the pivot.
// Elements less than the pivot are moved to the left side,
// and elements greater than or equal to the pivot stay on the right.
// Returns the final position of the pivot.
int partition_last_pivot(int array[], int left, int right) {

    int pivot_value = array[right]; // Choosing the last element as pivot
    int pivot_index = (left - 1);

    printarr(array, right+1, right, -1);

    for (int current = left; current <= right - 1; current++) {
        printarr(array, right+1, current, right);

        if (array[current] < pivot_value) {
            pivot_index++;

            printarr(array, right+1, pivot_index, current);
            swap_ints(&array[pivot_index], &array[current]);
            printarr(array, right+1, pivot_index, current);
        }
    }

    printarr(array, right+1, pivot_index + 1, right);
    swap_ints(&array[pivot_index + 1], &array[right]);
    printarr(array, right+1, pivot_index + 1, right);
    return (pivot_index + 1);
}

// Recursively sort the array using quicksort.
// The function divides the array around a pivot and sorts the partitions.
void quick_sort(int array[], int left, int right) {
    printarr(array, right+1, left, right);
    if (left < right) {
        int partition_index = partition_last_pivot(array, left, right);
        printf("partition index: %d\n", partition_index);
        printarr(array, right+1, partition_index, -1);
        quick_sort(array, left, partition_index - 1);
        quick_sort(array, partition_index + 1, right);
    }
}


int main() {
    int arr[] = {75, 21, 76, 85, 94, 38, 28, 92, 7, 45, 61, 25, 65, 2, 96, 4, 58, 12, 52, 29};

    int arrsize = sizeof(arr) / sizeof(arr[0]);

    // unsigned long long start, end;
    // unsigned int dummy;

    // 1. Warm up the CPU and serialize the instruction pipeline
    // _mm_lfence(); 
    // start = __rdtscp(&dummy);

    // ----------------------------------------------------
    // INSERT THE C CODE YOU WANT TO MEASURE HERE
    // Example: A basic arithmetic task
    quick_sort(arr, 0, arrsize-1);
    // ----------------------------------------------------

    // 2. Serialize pipeline again so trailing instructions finish before reading the clock
    // _mm_lfence(); 
    // end = __rdtscp(&dummy);

    // printf("Total CPU clock cycles consumed: %llu\n", (end - start));
    

    printarr(arr, arrsize, -1, -1);
    return 0;
}