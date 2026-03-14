//
// Created by Gemini CLI on 03/14/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include "utils/queue.h"

Queue queue_create(int capacity) {
    Queue queue;
    queue.capacity = capacity < 1 ? 1 : capacity;
    queue.head = 0;
    queue.tail = 0;
    queue.size = 0;
    queue.arr = calloc(queue.capacity, sizeof(void*));
    if (!queue.arr) {
        fprintf(stderr, "Failed to allocate queue memory during initialization\n");
        queue.capacity = 0;
    }
    return queue;
}

void queue_free(Queue *queue) {
    free(queue->arr);
    queue->arr = NULL;
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
    queue->capacity = 0;
}

int queue_is_empty(Queue *queue) {
    return queue->size == 0;
}

int queue_is_full(Queue *queue) {
    return queue->size == queue->capacity;
}

void queue_enqueue(Queue *queue, void* dataPtr) {
    if (queue_is_full(queue)) {
        int oldCapacity = queue->capacity;
        int newCapacity = oldCapacity * 2;
        void **newArr = malloc(sizeof(void*) * newCapacity);
        if (newArr == NULL) {
            fprintf(stderr, "queue error: Could not reallocate queue memory.\n");
            return;
        }

        // Copy elements in order from head to tail
        for (int i = 0; i < queue->size; i++) {
            newArr[i] = queue->arr[(queue->head + i) % oldCapacity];
        }

        free(queue->arr);
        queue->arr = newArr;
        queue->head = 0;
        queue->tail = queue->size;
        queue->capacity = newCapacity;
    }
    queue->arr[queue->tail] = dataPtr;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
}

void* queue_dequeue(Queue *queue) {
    if (queue_is_empty(queue)) {
        return NULL;
    }
    void* data = queue->arr[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    return data;
}
