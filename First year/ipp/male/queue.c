#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "handle-errors.h"

const size_t INIT_SIZE = 8;

struct Queue_ {
    size_t *array;
    size_t firstIndex, lastIndex, totalSize, usedSize;
};

void initQueue(Queue **queueDoublePtr) {
    Queue *buffer = malloc(sizeof(Queue));
    size_t *buffer2 = malloc(INIT_SIZE * sizeof(size_t));

    if(buffer == NULL || buffer2 == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }

    buffer->array = buffer2;
    buffer->usedSize = 0;
    buffer->totalSize = INIT_SIZE;
    buffer->firstIndex = 0;
    buffer->lastIndex = 0;

    *queueDoublePtr = buffer;
}

static size_t next(Queue *queuePtr, size_t index) {
    if(index == queuePtr->totalSize - 1) {
        return 0;
    }
    else {
        return index + 1;
    }
}

static bool isFull(Queue *queuePtr) {
    return queuePtr->firstIndex == next(queuePtr, queuePtr->lastIndex);
}

static void copy(Queue *queuePtr, size_t *newArray, size_t newSize) {
    size_t fixedIndex = queuePtr->firstIndex;
    for(size_t i = 0; i < queuePtr->usedSize; i++) {
        newArray[i] = queuePtr->array[fixedIndex];
        fixedIndex = next(queuePtr, fixedIndex);
    }
    free(queuePtr->array);
    queuePtr->array = newArray;
    queuePtr->firstIndex = 0;
    queuePtr->lastIndex = queuePtr->usedSize;
    queuePtr->totalSize = newSize;
}

static void expand(Queue *queuePtr) {
    if(isFull(queuePtr)) {
        size_t newSize = queuePtr->totalSize * 2;
        size_t *buffer = malloc(newSize * sizeof(size_t));
        if(buffer == NULL) {
            handleMemoryError(MEMORY_ERROR);
        }
        copy(queuePtr, buffer, newSize);
    }
}

void push(Queue *queuePtr, size_t indexToPush) {
    expand(queuePtr);
    size_t queueIndex = queuePtr->lastIndex;
    queuePtr->array[queueIndex] = indexToPush;
    queuePtr->lastIndex = next(queuePtr, queueIndex);
    queuePtr->usedSize++;
}

void pop(Queue *queuePtr) {
    queuePtr->usedSize--;
    queuePtr->firstIndex = next(queuePtr, queuePtr->firstIndex);
}

size_t first(Queue *queuePtr) {
    return queuePtr->array[queuePtr->firstIndex];
}

bool isEmpty(Queue *queuePtr) {
    return queuePtr->firstIndex == queuePtr->lastIndex;
}

void freeQueue(Queue *queuePtr) {
    if(queuePtr != NULL) {
        free(queuePtr->array);
    }
    free(queuePtr);
}