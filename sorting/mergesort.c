#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>

inline unsigned long long collect_cycles() {
    unsigned int ui;
    // __rdtscp returns the 64-bit time-stamp counter
    return __rdtscp(&ui);
}

// merge() combines two sorted halves of the array into one sorted section.
// array:         array containing the values to merge
// leftIndex:     index of the first element in the left half
// middleIndex:   index of the last element in the left half
// rightIndex:    index of the last element in the right half
void merge(int array[], int leftIndex, int middleIndex, int rightIndex) {
    int leftSize = middleIndex - leftIndex + 1; // size of left subarray
    int rightSize = rightIndex - middleIndex;   // size of right subarray
    int *leftArray = (int *)malloc(leftSize * sizeof(int));
    int *rightArray = (int *)malloc(rightSize * sizeof(int));

    // copy values from array into the temporary left and right subarrays
    for (int leftCopyIndex = 0; leftCopyIndex < leftSize; leftCopyIndex++)
        leftArray[leftCopyIndex] = array[leftIndex + leftCopyIndex];
    for (int rightCopyIndex = 0; rightCopyIndex < rightSize; rightCopyIndex++)
        rightArray[rightCopyIndex] = array[middleIndex + 1 + rightCopyIndex];

    int leftPos = 0, rightPos = 0, mergePos = leftIndex;
    // merge elements from leftArray and rightArray back into array in sorted order
    while (leftPos < leftSize && rightPos < rightSize) {
        if (leftArray[leftPos] <= rightArray[rightPos])
            array[mergePos++] = leftArray[leftPos++];
        else
            array[mergePos++] = rightArray[rightPos++];
    }
    while (leftPos < leftSize)
        array[mergePos++] = leftArray[leftPos++];
    while (rightPos < rightSize)
        array[mergePos++] = rightArray[rightPos++];
    
    free(leftArray);
    free(rightArray);
}

// mergeSort() sorts the array by dividing it into smaller parts,
// sorting the parts, and then merging them back together.
// arr:   array to sort
// left:  starting index of the current section
// right: ending index of the current section
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void printarr(int arr[], int size) {
    printf("[ ");
    for (int i = 0; i < size; i++) {
        if (i == size-1) {
            printf("%d ", arr[i]);
        } else {
            printf("%d, ", arr[i]);
        }
    }
    printf("]\n");
}

void run_benchmark(int arr[], void (*func)(int *, int, int), int size) {
    struct timespec start, end;
    
    // Start high-resolution timer
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Execute target function
    func(arr, 0, size - 1);
    
    // End high-resolution timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate elapsed time in seconds
    double time_taken = (end.tv_sec - start.tv_sec) + 
                        (end.tv_nsec - start.tv_nsec) / 1e9;
                        
    printf("Input Size (N): %7d | Time Taken: %f miliseconds\n", size, time_taken * 1000);
}

int main() {
    int arr[] = {75, 21, 76, 85, 94, 38, 28, 92, 7, 45, 61, 25, 65, 2, 96, 4, 58, 12, 52, 29};

    int arrsize = sizeof(arr) / sizeof(arr[0]);
    printarr(arr, arrsize);

    unsigned long long start, end;
    unsigned int dummy;

    // 1. Warm up the CPU and serialize the instruction pipeline
    _mm_lfence(); 
    start = __rdtscp(&dummy);

    // ----------------------------------------------------
    // INSERT THE C CODE YOU WANT TO MEASURE HERE
    // Example: A basic arithmetic task
    mergeSort(arr, 0, arrsize);
    // ----------------------------------------------------

    // 2. Serialize pipeline again so trailing instructions finish before reading the clock
    _mm_lfence(); 
    end = __rdtscp(&dummy);

    printf("Total CPU clock cycles consumed: %llu\n", (end - start));
    return 0;

    printarr(arr, arrsize);
}