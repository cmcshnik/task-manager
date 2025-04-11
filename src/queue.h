#pragma once

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node {
    int value_;
    struct Node* next_;
    struct Node* prev_;
} Node;

typedef struct Queue {
    Node* first;
    Node* last;
    bool is_empty;
} Queue;

// Create new queue instance.
// Returns NULL on error.
Queue* NewQueue(void);

// Free queue instance.
// Ignores NULL instance and NULL fields.
void FreeQueue(Queue* queue);

// Push element to a queue.
// Returns true on success, otherwise returns false and changes errno.
bool Push(Queue* queue, int elem);

// Get top queue element and store it into elem.
// Returns true on success, otherwise returns false and changes errno.
bool Front(const Queue* queue, int* elem);

// Pop element from a queue.
// Returns true on success, otherwise returns false and changes errno.
bool Pop(Queue* queue);

// Checks if a queue is empty.
bool IsEmpty(const Queue* queue);

// Printing queue
// void PrintQueue(Queue* queue);