//
// Created by Gemini CLI on 03/14/2026.
//
#ifndef QUEUE_H
#define QUEUE_H

//  Circular void* queue for BFS scent mapping and reachability logic
typedef struct {
    int head, tail;
    int size, capacity;
    void **arr;
} Queue;

//  Initializes a circular queue with an initial capacity
Queue queue_create(int capacity);

//  Frees the underlying pointer array
void queue_free(Queue *queue);

//  Returns 1 if no items are present
int queue_is_empty(Queue *queue);

//  Returns 1 if the queue is full (before automatic growth)
int queue_is_full(Queue *queue);

//  Adds a pointer to the back of the queue, auto-doubles capacity if full
void queue_enqueue(Queue *queue, void* dataPtr);

//  Returns and removes the front item from the queue
void* queue_dequeue(Queue *queue);

#endif //QUEUE_H
