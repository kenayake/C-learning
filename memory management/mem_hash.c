#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int TABLE_SIZE = 6;

// Represents a single allocation entry in the hash table chain
// Stores the allocated address, its size, and a pointer to the next entry in the collision chain
typedef struct AllocationEntry {
    void* address;          // Pointer to the allocated memory block
    size_t size;            // Size of the allocated memory in bytes
    struct AllocationEntry* next;  // Pointer to next entry for handling hash collisions
} AllocationEntry;

// Hash table registry for tracking all memory allocations
// Uses separate chaining with TABLE_SIZE buckets to handle collisions
typedef struct {
    AllocationEntry** buckets;  // Array of pointers to entry chains
    int entries;
} MemoryRegistry;

void resize_registry(MemoryRegistry* registry, int* TABLE_SIZE);

MemoryRegistry initialize_registry() {
    MemoryRegistry registry;

    registry.entries = 0;
    registry.buckets = calloc(TABLE_SIZE, sizeof(AllocationEntry));
    return registry;
}

// Computes hash index for a given pointer address
// Uses modulo arithmetic to map any pointer to a bucket within TABLE_SIZE
// Safely casts pointer to uintptr_t before performing the modulo operation
unsigned int hash(void* ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    double multiplier = (sqrt(5) - 1) / 2;
    double modulo = fmod( addr * multiplier, 1 );
    return (unsigned int)(floor(TABLE_SIZE *  modulo) );
}

// Registers a new memory allocation in the registry
// Creates a new AllocationEntry and inserts it at the head of the appropriate hash chain
// Time complexity: O(1) average case since we insert at the head
void register_allocation(MemoryRegistry* registry, void* ptr, size_t size) {
    unsigned int index = hash(ptr);
    AllocationEntry* new_entry = malloc(sizeof(AllocationEntry));
    new_entry->address = ptr;
    new_entry->size = size;
    
    // Insert at head of the chain for constant-time insertion
    new_entry->next = registry->buckets[index];
    registry->buckets[index] = new_entry;
    registry->entries++;

    // Check for load factor exceeded
    float load_factor = (float) registry->entries / TABLE_SIZE;
    if (load_factor > 0.75) {
        resize_registry(registry, &TABLE_SIZE);
    }
}

// Searches for an allocation record by its memory address
// Traverses the hash chain at the computed bucket index to find a matching address
// Returns pointer to the AllocationEntry if found, or NULL if the address is not registered
AllocationEntry* find_allocation(MemoryRegistry* registry, void* ptr) {
    unsigned int index = hash(ptr);
    AllocationEntry* entry = registry->buckets[index];
    
    // Iterate through the collision chain
    while (entry != NULL) {
        if (entry->address == ptr) return entry;  // Found a match
        entry = entry->next;
    }
    return NULL; // Address not found in registry
}

void remove_allocation(MemoryRegistry* registry, void* ptr) {
    unsigned int index = hash(ptr);
    AllocationEntry* entry = registry->buckets[index];
    AllocationEntry* prevEntry = NULL;

    while (entry != NULL) {
        if (entry->address == ptr) {
            if (prevEntry != NULL) {
                prevEntry->next = entry->next;
            } else {
                registry->buckets[index] = entry->next;
            }
            free(entry->address);
            free(entry);
            registry->entries--;
            break;
        };
        prevEntry = entry;
        entry = entry->next;
    }
}

void resize_registry(MemoryRegistry* registry, int* TABLE_SIZE) {
    int oldTableSize = *TABLE_SIZE;
    *TABLE_SIZE = oldTableSize * 2;

    AllocationEntry** oldBucket = registry->buckets;
    registry->buckets = calloc(*TABLE_SIZE, sizeof(AllocationEntry));

    for (int i = 0; i < oldTableSize; i++) {
        if (oldBucket[i] != NULL) {
            AllocationEntry* entry = oldBucket[i];
    
            while (entry != NULL) {
                register_allocation(registry, entry->address, entry->size);
                entry = entry->next;
            }
        }
    }

    free(oldBucket);
}

int main() {
    MemoryRegistry registry = initialize_registry();

    int* a = malloc(sizeof(int));
    int* b = malloc(sizeof(int) * 5);
    char* c = malloc(32);
    double* d = malloc(sizeof(double) * 3);
    float* e = malloc(sizeof(float) * 10);

    // Register them
    register_allocation(&registry, a, sizeof(int));
    register_allocation(&registry, b, sizeof(int) * 5);
    register_allocation(&registry, c, 32);
    register_allocation(&registry, d, sizeof(double) * 3);
    register_allocation(&registry, e, sizeof(float) * 10);

    AllocationEntry* result;

    result = find_allocation(&registry, c);
    if (result)
        printf("Found c: %p (%zu bytes)\n", result->address, result->size);
    else
        printf("c not found\n");

    result = find_allocation(&registry, b);
    if (result)
        printf("Found b: %p (%zu bytes)\n", result->address, result->size);
    else
        printf("b not found\n");

    // Test searching for an unregistered pointer
    int dummy;
    result = find_allocation(&registry, &dummy);

    if (result)
        printf("Dummy found\n");
    else
        printf("Dummy not found (expected)\n");
    
    remove_allocation(&registry, c);

    result = find_allocation(&registry, c);

    if (result)
        printf("Error: c still exists.\n");
    else
        printf("c successfully removed.\n");

    void* ptrs[20];

    for (int i = 0; i < 20; i++) {
        ptrs[i] = malloc(i + 1);
        register_allocation(&registry, ptrs[i], i + 1);
    }

    for (int i = 0; i < 20; i++) {
        if (find_allocation(&registry, ptrs[i]))
            printf("Found %d\n", i);
        else
            printf("Missing %d\n", i);
    }
}