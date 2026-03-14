//
// Created by Alan Pitcher on 1/9/2026.
//
#ifndef STACK_H
#define STACK_H

//  Dynamic void* stack for graph traversal and DFS maze generation
typedef struct {
    int top;
    int capacity;
    void **arr;
} Stack;

//  Initializes a stack with a specific starting capacity
Stack stack_create(int capacity);

//  Frees the underlying pointer array
void stack_free(Stack *stack);

//  Returns 1 if no items are present
int stack_is_empty(Stack *stack);

//  Returns 1 if the stack reached capacity (before automatic growth)
int stack_is_full(Stack *stack);

//  Pushes a pointer onto the stack, auto-doubles capacity if full
void stack_push(Stack *stack, void* dataPtr);

//  Returns and removes the top item from the stack
void* stack_pop(Stack *stack);


#endif //STACK_H
