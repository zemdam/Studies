#include <stdint.h>
#include <stdlib.h>
#include "handle-errors.h"
#include "arrays.h"

static const size_t SIZE_MULTIPLIER = 2;

// Structure which hold dynamic array.
struct Array_ {
    size_t *array;
    size_t usedSize;
    size_t maxSize;
};

// Function initializes dynamic array. If no memory is available
// program ends via handleMemoryError function.
//  arrayDoublePtr - double pointer to Array which will hold dynamic array
//  initSize       - initialization size of dynamic array
void initArray(Array **arrayDoublePtr, size_t initSize) {
    size_t *buffer;
    Array *buffer2;

    buffer2 = malloc(sizeof(Array));
    // If malloc returns NULL it means that no memory is available
    if(buffer2 == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }
    else {
        *arrayDoublePtr = buffer2;
        (*arrayDoublePtr)->array = NULL;
    }

    buffer = malloc(initSize * sizeof(size_t));
    if(buffer == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }
    else {
        (*arrayDoublePtr)->array = buffer;
        (*arrayDoublePtr)->usedSize = 0;
        (*arrayDoublePtr)->maxSize = initSize;
    }
}

// Function expands dynamic array if it is full. If no memory is available
// function behave like initArray function. When total size of dynamic array
// would be larger than SIZE_MAX function returns OTHER_ERROR.
//  arrayPtr - pointer to Array which holds dynamic array that could be expanded
Error expandArray(Array *arrayPtr) {
    if(arrayPtr->maxSize == arrayPtr->usedSize) {
        // Checks if new size would be bigger than SIZE_MAX.
        if(SIZE_MAX / SIZE_MULTIPLIER < arrayPtr->maxSize) {
            return OTHER_ERROR;
        }
        else {
            size_t *buffer;
            arrayPtr->maxSize *= SIZE_MULTIPLIER;
            buffer = realloc(arrayPtr->array,
                             arrayPtr->maxSize * sizeof(size_t));
            if(buffer == NULL) {
                handleMemoryError(MEMORY_ERROR);
            }
            else {
                arrayPtr->array = buffer;
                return NO_ERROR;
            }
        }
    }
    return NO_ERROR;
}

// Function inserts element to dynamic array. Returns Error number
// if error occurs.
//  arrayPtr - pointer to dynamic array holder
//  number   - value which will be inserted to dynamic array
Error insertToArray(Array *arrayPtr, size_t number) {
    Error errorNumber = expandArray(arrayPtr);
    if(errorNumber == NO_ERROR) {
        arrayPtr->array[arrayPtr->usedSize] = number;
        arrayPtr->usedSize++;
    }
    return errorNumber;
}

// Function changes element of dynamic array.
//  arrayPtr - pointer to holder of dynamic array
//  index    - index of element which wille changed
//  number   - value which will replace previous element
void changeArray(Array *arrayPtr, size_t index, size_t number) {
    arrayPtr->array[index] = number;
}

void freeArray(Array *arrayPtr) {
    // Checks if arrayPtr was initialized so function won't crash program
    if(arrayPtr != NULL) {
        free(arrayPtr->array);
    }
    free(arrayPtr);
}

// Function returns used size of dynamic array.
size_t sizeOfArray(Array *pointerToArray) {
    return pointerToArray->usedSize;
}

// Function returns element of dynamic array at given location.
//  arrayPtr - pointer to holder of dynamic array
//  index    - index of element which will be returned
size_t elementOfArray(Array *arrayPtr, size_t index) {
    return arrayPtr->array[index];
}

// Function checks if two dynamic arrays have the same number of elements
// and a[i] <= b[i] for all i.
//  max          - pointer to holder of b[]
//  arrayToCheck - pointer to holder of a[]
bool isSameSize(Array *max, Array *arrayToCheck) {
    if(max->usedSize == arrayToCheck->usedSize) {
        for(size_t i = 0; i < max->usedSize; i++) {
            if(arrayToCheck->array[i] > max->array[i]) {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}