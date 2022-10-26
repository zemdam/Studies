#ifndef IPPZAD1_ARRAYS_H
#define IPPZAD1_ARRAYS_H

#include <stdbool.h>
#include "errors.h"

extern void initArray(Array **arrayDoublePtr, size_t initSize);
extern Error expandArray(Array *arrayPtr);
extern Error insertToArray(Array *arrayPtr, size_t number);
extern void freeArray(Array *arrayPtr);
extern size_t sizeOfArray(Array *pointerToArray);
extern size_t elementOfArray(Array *arrayPtr, size_t index);
extern void changeArray(Array *arrayPtr, size_t index, size_t number);
extern bool isSameSize(Array *max, Array *arrayToCheck);

#endif //IPPZAD1_ARRAYS_H
