#include "queue.h"


Queue* NewQueue(void) {
    Queue* res = malloc(sizeof(Queue));
    if (!res) {
        errno = ENOMEM;
        return NULL;
    }

    res->first = NULL;
    res->last = NULL;
    res->is_empty = true;

    return res;
}

void FreeQueue(Queue* queue) {
    if (queue == NULL) {
        return;
    }

    if (queue->first == NULL) {
        free(queue);
        queue = NULL;
        return;
    }

    Node* cur_node = queue->first;
    Node* next_node;

    while (true) {
        next_node = cur_node->next_;
        free(cur_node);

        if (next_node == NULL) {
            break;
        }

        cur_node = next_node;
    }

    free(queue);
    queue = NULL;
}

bool Push(Queue* queue, int elem) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    Node* node = malloc(sizeof(Node));
    if (!node) {
        errno = ENOMEM;
        return false;
    }

    node->value_ = elem;
    node->next_ = NULL;

    if (queue->first == NULL) {
        queue->first = node;
    } else {
        queue->last->next_ = node;
    }

    node->prev_ = queue->last;
    queue->last = node;
    queue->is_empty = false;
    return true;
}

bool Front(const Queue* queue, int* elem) {
    if (queue == NULL || elem == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue->first == NULL) {
        return false;
    }
    
    *elem = queue->first->value_;
    return true;
}

bool Pop(Queue* queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue->first == queue->last) {
        free(queue->first);
        queue->first = NULL;
        queue->last = NULL;
        queue->is_empty = true;
    } else {
        Node* next_node = queue->first->next_;
        free(queue->first);
        queue->first = next_node;
    }

    return true;
}

bool IsEmpty(const Queue* queue) {
    return queue == NULL || queue->is_empty;
}

// void PrintQueue(Queue* queue) {
//     if (queue == NULL) {
//         return;
//     }

//     Node* cur_node = queue->first;

//     while (cur_node != NULL) {
//         printf("%d ", cur_node->value_);
//         cur_node = cur_node->next_;
//     }

//     fputc('\n', stdout);
// }