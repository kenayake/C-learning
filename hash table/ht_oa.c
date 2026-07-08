#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct HashMap {
  int capacity;
  int size;
  int *map;
} HashMap;

unsigned int hash(int key, int table_size) { return key % table_size; }

HashMap *init(int size) {
  int *map = calloc(size, sizeof(int));
  if (map == NULL) {
    exit(EXIT_FAILURE);
  }

  HashMap *hashMap = malloc(sizeof(HashMap));
  if (hashMap == NULL) {
    free(map);
    exit(EXIT_FAILURE);
  }

  hashMap->capacity = size;
  hashMap->size = 0;
  hashMap->map = map;

  return hashMap;
}

void insert(HashMap* map, int key) {
    int index = hash(key, map->capacity);
    int* arr = map->map;

    while (arr[index] != 0) {
        index = (index + 1) % map->capacity;
    }
    arr[index] = key;
    map->size++;
}

void printHashMap(HashMap* map) {
    printf("HashMap capacity=%d size=%d\n", map->capacity, map->size);
    for (int i = 0; i < map->capacity; ++i) {
        if (map->map[i] != 0) {
            printf("[%d] %d\n", i, map->map[i]);
        } else {
            printf("[%d] (empty)\n", i);
        }
    }
}

int main() {
  HashMap *hashMap = init(10);
  insert(hashMap, 12);
  printHashMap(hashMap);
  insert(hashMap, 22);
  printHashMap(hashMap);
  insert(hashMap, 32);
  printHashMap(hashMap);

  return 0;
}