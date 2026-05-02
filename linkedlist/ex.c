#include <stdio.h>
#include <stdlib.h>
// A node contains the value and a pointer to the next node
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// ex 3
Node* last_node(Node* head) {
    Node *cursor = head;
    printf("%p\n", head);
    while (cursor != NULL) {
        if (cursor->next == NULL) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

// ex1
int pop_front(Node **head) {
    int data = (*head)->data;
    Node *next = (*head)->next;
    
    free(*head);
    *head = next;
    return data;
}

// ex2
void print_list(Node *head) {
    Node *cursor = head;
    printf("(");
    while (cursor != NULL) {
        if (cursor->next != NULL) {
            printf("%d,", cursor->data);
        } else {
            printf("%d", cursor->data);
        }
        cursor = cursor->next;
    }
    printf(")\n");
}

Node* create_node(int value) {
    // Request space for one Node from the heap
    Node* new_node = (Node*)malloc(sizeof(Node));
    
    // Always check if malloc succeeded
    if (new_node == NULL) {
        return NULL;
    }
    
    new_node->data = value;
    new_node->next = NULL; // Terminate the list here
    return new_node;
}

void push_front(Node** head, int value) {
    Node* new_node = create_node(value);
    
    // Point the new node to the current start
    new_node->next = *head;
    
    // Update the head pointer to the new node
    *head = new_node;
}

void free_list(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;      // Store current node
        head = head->next; // Move to next node
        free(temp);       // Free stored node
    }
}

int main() {
    Node *linked_list = create_node(5);
    
    print_list(linked_list);
    
    printf("%d\n", pop_front(&linked_list));
    
    print_list(linked_list);
    
    printf("%d\n", last_node(linked_list)->data);
    
    return 0;
}