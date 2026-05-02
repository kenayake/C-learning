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

void append(Node** node, char* value) {
    Node* new = malloc(sizeof(Node));

    if (new == NULL) {
        exit(EXIT_FAILURE);
    }

    new->index = (*node)->index;
    new->value = value;
    new->next = NULL;

    (*node)->next = new;
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

// Ex 1
HashMap initialize(int size) {
    int capacity = (size/0.75) + 1;

    Node **table = malloc(capacity * sizeof(Node*));
    if (table == NULL) {
        exit(EXIT_FAILURE);
    }

    HashMap map;
    map.capacity = capacity;
    map.size = size;
    map.table = table;

    return map;
}

void insert(HashMap *map, char* key) {
    int index = hash(key, map->capacity);
    if (map->table[index] != NULL) {
        append(&map->table[index], key);
    } else {
        map->table[index] = create_node(index, key);
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
    HashMap hashMap = initialize(10);

    insert(&hashMap, "god");
    insert(&hashMap, "god");

    pretty_print(&hashMap);

    return 1;
}