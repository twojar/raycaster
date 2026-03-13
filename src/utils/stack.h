//
// Created by Alan Pitcher on 1/9/2026.
//
// Stack data structure implementation
// Used for DFS to generate a random maze
// Other purposes later in the future?
#ifndef STACK_H
#define STACK_H

typedef struct {
    int top;
    int capacity;
    void **arr;
} Stack;

Stack stack_create(int capacity);
void stack_free(Stack *stack);
int stack_is_empty(Stack *stack);
int stack_is_full(Stack *stack);
void stack_push(Stack *stack, void* dataPtr);
void* stack_pop(Stack *stack);


#endif //STACK_H
