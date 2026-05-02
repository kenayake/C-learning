#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct Node {
  int data;
  struct Node *left;
  struct Node *right;
  int height;
} Node;

void print_tree_topdown(Node *root, int space);

void triggerRebalance(Node **root);

int get_height(Node *node);

Node *create_node(int value) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  new_node->data = value;
  new_node->left = NULL;
  new_node->right = NULL;
  new_node->height = 0;
  return new_node;
}

int get_max(int a, int b) { return (a > b) ? (a) : (b); }

Node* minValueNode(Node* root) {
    if (root->left == NULL) {
        return root;
    } else {
        return minValueNode(root->left);
    }
}

Node *rebalance_rtl(Node *node) {
  Node *right_child = node->right;
  Node *left_subchild = right_child->left;

  node->right = left_subchild;
  right_child->left = node;

  // Update heights
  node->height = get_max(get_height(node->left), get_height(node->right)) + 1;
  right_child->height =
      get_max(get_height(right_child->left), get_height(right_child->right)) +
      1;

  return right_child;
}

Node *rebalance_ltr(Node *node) {
  Node *left_child = node->left;
  Node *right_subchild = left_child->right;

  node->left = right_subchild;
  left_child->right = node;

  // Update heights
  node->height = get_max(get_height(node->left), get_height(node->right)) + 1;
  left_child->height =
      get_max(get_height(left_child->left), get_height(left_child->right)) + 1;

  return left_child;
}

// ex2
int get_height(Node *node) {
  if (node == NULL) {
    return -1;
  }
  return node->height;
}

// ex3
int get_balance(Node *node) {
  int left_height = get_height(node->left);
  int right_height = get_height(node->right);

  return left_height - right_height;
}

// ex1
void insert(Node **root, int value) {
  if (value > (*root)->data) {
    // exit condition
    if ((*root)->right == NULL) {
      // root is now current node at recursion iteration
      Node *new_node = create_node(value);
      (*root)->right = new_node;

    } else {
      insert(&(*root)->right, value);
    }
  } else {
    if ((*root)->left == NULL) {
      Node *new_node = create_node(value);
      (*root)->left = new_node;

    } else {
      insert(&(*root)->left, value);
    }
  }
  (*root)->height =
      get_max(get_height((*root)->left), get_height((*root)->right)) + 1;
  triggerRebalance(root);
}

void triggerRebalance(Node **node) {
  int balance = get_balance(*node);
  if (balance > 1) {
    // left bigger
    Node *left_node = (*node)->left;
    int inner_balance = get_balance(left_node);

    if (inner_balance < 0) { // inner left smaller
      // Rebalance inner left
      (*node)->left = rebalance_rtl(left_node);
    }

    // Rebalance left side
    *node = rebalance_ltr(*node);

  } else if (balance < -1) {
    // right bigger
    Node *right_node = (*node)->right;
    int inner_balance = get_balance(right_node);

    if (inner_balance > 0) { // inner right smaller
      // Rebalance inner right
      (*node)->right = rebalance_ltr(right_node);
    }

    // Rebalance right side
    *node = rebalance_rtl(*node);
  }
}

Node *search(Node *node, int num) {
  if (node == NULL) {
    return NULL; 
  }
  if (node->data == num) {
    return node;
  }

  if (node->data < num) {
    return search(node->right, num);
  } else {
    return search(node->left, num);
  }
}

Node* delete(Node* node, int num) {
    if (node == NULL) {
        exit(EXIT_SUCCESS);
    }

    if (num > node->data) {
        node->right = delete(node->right, num);
    } else if (num < node->data) {
        node->left = delete(node->left, num);
    }
    
    if (node->data == num) {
        // Case 1: Node is a leaf
        if (node->height == 0) {
            free(node);
            return NULL;
        }

        // Case 2: Node has one child
        if ((node->left != NULL) != (node->right != NULL)) {
            Node* child = (node->left != NULL) ? node->left : node->right;
            free(node);
            return child;
        }

        // Case 3: Node has 2 children
        if (node->left != NULL && node->right != NULL) {
            Node* right_subchild = minValueNode(node->right);
            node->data = right_subchild->data;
            node->right = delete(node->right, right_subchild->data);
            return node;
        }
    }

    return node;
}

void print_pretty(Node *root, int level, bool *is_left, int depth) {
  if (root == NULL)
    return;

  // Process Right subtree (appears on top)
  if (root->right) {
    is_left[depth] = false;
    print_pretty(root->right, level + 1, is_left, depth + 1);
  }

  // Print current node
  for (int i = 0; i < depth; i++) {
    if (i == depth - 1) {
      // Branching point
      printf(is_left[i] ? " └── " : " ┌── ");
    } else {
      // Vertical connectors for parents with pending children
      // If the parent at this depth was a right child, and we are now
      // printing a left child (or vice versa), we draw the path.
      if (is_left[i] != is_left[i + 1]) {
        printf(" │   ");
      } else {
        printf("     ");
      }
    }
  }

  printf("%d\n", root->data);

  // Process Left subtree (appears on bottom)
  if (root->left) {
    is_left[depth] = true;
    print_pretty(root->left, level + 1, is_left, depth + 1);
  }
}

// Wrapper function
void display_tree(Node *root) {
  if (root == NULL)
    return;
  bool path_map[64] = {false}; // Tracks if we took a left turn at each depth
  print_pretty(root, 0, path_map, 0);
}

int main() {

  Node *bst = create_node(20);
  //   display_tree(bst);

  insert(&bst, 15);
  //   display_tree(bst);
  insert(&bst, 13);
  //   display_tree(bst);
  insert(&bst, 10);
  //   display_tree(bst);
  insert(&bst, 7);
  //   display_tree(bst);
  insert(&bst, 6);
  //   display_tree(bst);
  insert(&bst, 19);
  insert(&bst, 12);
  insert(&bst, 14);
  insert(&bst, 31);

  display_tree(bst);

  printf("%d", search(bst, 19)->data);

  bst = delete(bst, 15);

  display_tree(bst);

  return 0;
}