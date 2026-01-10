//
// Created by there on 1/9/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack stack_create(int capacity) {
    Stack stack;
    stack.capacity = capacity < 1 ? 1 : capacity;
    stack.top = 0;
    stack.arr = calloc(capacity, sizeof(void*));
    if (!stack.arr) {
        fprintf(stderr, "Failed to allocate stack memory during initialization\n");
        stack.capacity = 0;
    }
    return stack;
}

void stack_free(Stack *stack) {
    free(stack->arr);
    stack->arr = NULL;
    stack->top = 0;
    stack->capacity = 0;
}

int stack_is_empty(Stack *stack) {
    return stack->top == 0;
}


int stack_is_full(Stack *stack) {
    return stack->top == stack->capacity;
}

void stack_push(Stack *stack, void* dataPtr) {
    if (stack_is_full(stack)) {
        int newCapacity = stack->capacity * 2;
        void **temp = realloc(stack->arr, sizeof(void*) * newCapacity);
        if (temp == NULL) {
            fprintf(stderr,"stack error: Could not reallocate stack memory.\n");
            return;
        }
        stack->arr = temp;
        stack->capacity = newCapacity;
    }
    stack->arr[stack->top++] = dataPtr;
}

void* stack_pop(Stack *stack) {
    if (stack_is_empty(stack)) {
        return NULL;
    }
    return stack->arr[--stack->top];
}