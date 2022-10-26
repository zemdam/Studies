#ifndef IPPZAD1_QUEUE_H
#define IPPZAD1_QUEUE_H

#include "errors.h"

extern void initQueue(Queue **queueDoublePtr);
extern void push(Queue *queuePtr, size_t index);
extern void pop(Queue *queuePtr);
extern size_t first(Queue *queuePtr);
extern bool isEmpty(Queue *queuePtr);
extern void freeQueue(Queue *queuePtr);

#endif //IPPZAD1_QUEUE_H
