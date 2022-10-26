#include <stdlib.h>
#include "handle-errors.h"
#include "labyrinth.h"
#include "bitarrays.h"

struct Labyrinth_{
    Array *size;
    size_t startIndex;
    size_t targetIndex;
    BitArray *walls;
};

Error initLabyrinth(Labyrinth **labyrinthDoublePtr) {
    Labyrinth *buffer = malloc(sizeof(Labyrinth));

    if(buffer == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }
    else {
        buffer->size = NULL;
        buffer->startIndex = 0;
        buffer->targetIndex = 0;
        buffer->walls = NULL;
        *labyrinthDoublePtr = buffer;
    }
    return NO_ERROR;
}

void setLabyrinthSize(Labyrinth *labyrinthPtr, Array *size) {
    labyrinthPtr->size = size;
}

void setLabyrinthStart(Labyrinth *labyrinthPtr, Array *start) {
    size_t startIndex = getBitIndex(start, labyrinthPtr->size);
    labyrinthPtr->startIndex = startIndex;
}

void setLabyrinthTarget(Labyrinth *labyrinthPtr, Array *target) {
    size_t targetIndex = getBitIndex(target, labyrinthPtr->size);
    labyrinthPtr->targetIndex = targetIndex;
}

void setLabyrinthWalls(Labyrinth *labyrinthPtr, BitArray *walls) {
    labyrinthPtr->walls = walls;
}

Array *getLabyrinthSize(Labyrinth *labyrinthPtr) {
    return labyrinthPtr->size;
}

size_t getLabyrinthStart(Labyrinth *labyrinthPtr) {
    return labyrinthPtr->startIndex;
}

size_t getLabyrinthTarget(Labyrinth *labyrinthPtr) {
    return labyrinthPtr->targetIndex;
}

BitArray *getLabyrinthWalls(Labyrinth *labyrinthPtr) {
    return labyrinthPtr->walls;
}