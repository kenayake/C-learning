#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Node {
    unsigned int index;
    char* value;
    struct Node* next;
} Node;

Node* create_node(int index, char* value) {
    // Request space for one Node from the heap
    Node* new_node = (Node*)malloc(sizeof(Node));
    
    // Always check if malloc succeeded
    if (new_node == NULL) {
        return NULL;
    }
    
    new_node->index = index;
    new_node->value = value;
    new_node->next = NULL; // Terminate the list here
    return new_node;
}

Node* append(Node* node, char* value) {
    Node* new = malloc(sizeof(Node));

    if (new == NULL) {
        exit(EXIT_FAILURE);
    }

    Node* cursor = node;
    while (cursor != NULL) {
        if (cursor->next == NULL) {
            new->index = node->index;
            new->value = value;
            new->next = NULL;
            cursor->next = new;
            return node;
        }
        cursor = cursor->next;
    }

    
}

typedef struct {
    int capacity;
    int size;
    Node **table;
} HashMap;

unsigned int hash(char* key, int table_size) {
    unsigned int hash_val = 0;
    int length = strlen(key);
    // for (int i = length - 1; i > 0; i--) {
    //     hash_val = hash_val + *key++ * pow(31, i);
    // }
    while (*key) {
        hash_val = (hash_val << 5) + *key++;
    }
    return hash_val % table_size;
}

double get_load_factor(HashMap* map) {
    if (map->capacity == 0) return 0;
    return (double)map->size / map->capacity;
}

void resize(HashMap *map) {

    // Get new capacity
    int capacity = map->capacity * 2;

    // Create new array with updated capacity
    Node **table = calloc(capacity, sizeof(Node*));
    if (table == NULL) {
        exit(EXIT_FAILURE);
    }

    // Iterate all values of previous array
    for (int i = 0; i < map->capacity; i++) {
        Node* current = map->table[i];
        while (current != NULL) {

            char* key = current->value;
            int index = hash(key, capacity);

            if (table[index] != NULL) {
                table[index] = append(table[index], key);
            } else {
                table[index] = create_node(index, key);
            }
            
            current = current->next;
        }
    }

    free(map->table);
    map->table = table;
    map->capacity = capacity;
}

// Ex 1
HashMap* initialize(int size) {
    int capacity = (size/0.75) + 1;

    Node **table = calloc(capacity, sizeof(Node*));
    if (table == NULL) {
        exit(EXIT_FAILURE);
    }
    HashMap *map = malloc(sizeof(HashMap));
    if (map == NULL) {
        free(table);
        exit(EXIT_FAILURE);
    }

    map->capacity = capacity;
    map->size = 0;
    map->table = table;

    return map;
}

void insert(HashMap *map, char* key) {
    int index = hash(key, map->capacity);
    if (map->table[index] != NULL) {
        map->table[index] = append(map->table[index], key);
    } else {
        map->table[index] = create_node(index, key);
    }

    map->size++;

    // Resize if overloaded
    if (get_load_factor(map) >  0.75) {
        resize(map);
    }
}

Node* get(HashMap* map, char* string) {
    unsigned int index = hash(string, map->capacity);

    Node** table = map->table;

    if (table[index] == NULL) {
        exit(EXIT_FAILURE);
    } else {
        Node* cursor = table[index];
        while (cursor != NULL) {
            if (strcmp(cursor->value, string) == 0) {
                return cursor;
            }
            cursor = cursor->next;
        }
        exit(EXIT_FAILURE);
    }
}



void pretty_print(HashMap* map) {
    for (int i = 0; i < map->capacity; i++) {
        printf("Index %d: ", i);
        Node* current = map->table[i];
        while (current != NULL) {
            printf("(%d, %s) -> ", current->index, current->value);
            current = current->next;
        }
        printf("NULL\n");
    }
}


int main() {
    HashMap *hashMap = initialize(10);
    hashMap->capacity = 10;

    insert(hashMap, "dog");
    insert(hashMap, "god");
    insert(hashMap, "fod");
    insert(hashMap, "goc");
    insert(hashMap, "gdo");
    insert(hashMap, "tor");
    insert(hashMap, "rot");
    insert(hashMap, "lov");

    Node* waow = get(hashMap, "goc");

    printf("%f", get_load_factor(hashMap));

    pretty_print(hashMap);

    return 1;
}