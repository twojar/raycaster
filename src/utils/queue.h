//
// Created by Alan Pitcher on 03/14/2026.
//
// Queue data structure implementation
// Used for BFS in entity pathfinding and scent mapping
#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    int head;
    int tail;
    int size;
    int capacity;
    void **arr;
} Queue;

Queue queue_create(int capacity);
void queue_free(Queue *queue);
int queue_is_empty(Queue *queue);
int queue_is_full(Queue *queue);
void queue_enqueue(Queue *queue, void* dataPtr);
void* queue_dequeue(Queue *queue);

#endif //QUEUE_H
