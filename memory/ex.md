# C Memory Management Exercises

## 1. The Lifetime Trap (Stack vs. Heap)

Write a function that returns a pointer to an integer.

- **Version A**: Declare an int locally on the stack, initialize it, and return its address. Compile and observe the warning. Print the value in main to see the "dangling pointer" effect (garbage data).
- **Version B**: Use `malloc` to allocate the int on the heap, initialize it, and return the pointer. Print the value in main and ensure you `free` the memory to avoid leaks.

## 2. Manual Memory Inspection

Allocate an array of 10 integers using `malloc`.

- Iterate through the array using pointer arithmetic (`*(ptr + i)`) rather than array indexing (`ptr[i]`).
- Verify the addresses of each element; observe that they are contiguous.
- Force a segmentation fault by accessing `ptr[1000000]` to understand the boundary between allocated heap space and protected memory.

## 3. The "Memory Leak" Stress Test

Write a loop that runs 1,000,000 times. Inside the loop, `malloc` 1KB of memory but do not free it.

- Run the program and monitor your system's memory usage (using `top` on Linux or Task Manager on Windows).
- Observe how the OS reclaims this memory only after the process terminates.
- **Refinement**: Use Valgrind to run your executable: `valgrind --leak-check=full ./your_program`. Learn to interpret the "definitely lost" report.

## 4. Stack Overflow Simulation

Write a recursive function that increments a counter and calls itself indefinitely.

- Observe the segmentation fault (or stack overflow) once the stack limit is reached.
- Contrast this with the heap: Attempt to allocate memory in a loop using `malloc` until it returns `NULL`. This demonstrates the hard limit of your physical RAM vs. the thread-local stack limit (usually 8MB on Linux).

## 5. String Manipulation Hazards

Allocate a char buffer on the heap using `malloc(5 * sizeof(char))`.

- Attempt to copy a string longer than 4 characters (e.g., "Hello") into the buffer using `strcpy`.
- Observe the heap corruption.
- **Fix**: Implement a robust version using `calloc` (to zero-initialize) and `strncpy` (to enforce bounds), followed by an explicit `free`.