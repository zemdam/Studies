#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "bfs.h"
#include "queue.h"
#include "labyrinth.h"
#include "bitarrays.h"
#include "arrays.h"

// Checks if a way was found.
static bool isCompleted(size_t targetIndex, size_t locationIndex) {
    return targetIndex == locationIndex;
}

static bool isWall(BitArray *walls, size_t locationIndex) {
    return getBitFixed(walls, locationIndex);
}

static void markWall(BitArray *walls, size_t locationIndex) {
    setBitFixed(walls, locationIndex);
}

// Function generates index of field which is connected current location in
// one dimension if it is possible. Coordinate in this dimension is always
// greater by 1.
//  location      - pointer to holder of dynamic array which holds current coordinates
//  size          - pointer to holder of dynamic array with max coordinates
//  indexToChange - number of dimension to change
static size_t plusNeighbour(Array *location, Array *size, size_t indexToChange) {
    size_t neighbourIndex;
    size_t locationElement = elementOfArray(location, indexToChange);
    size_t sizeElement = elementOfArray(size, indexToChange);
    size_t change = 0;
    if(locationElement < sizeElement) {
        change = 1;
    }
    // Temporary changes coordinates.
    changeArray(location, indexToChange, locationElement + change);
    neighbourIndex = getBitIndex(location, size);
    // Restore coordinates.
    changeArray(location, indexToChange, locationElement);
    return neighbourIndex;
}

// Function do the same as plusNeighbour function but coordinate in one dimension
// is lower by 1 instead.
static size_t minusNeighbour(Array *location, Array *size, size_t indexToChange) {
    size_t neighbourIndex = indexToChange;
    size_t locationElement = elementOfArray(location, indexToChange);
    size_t change = 0;
    if(locationElement > 1) {
        change = 1;
    }

    changeArray(location, indexToChange, locationElement - change);
    neighbourIndex = getBitIndex(location, size);
    changeArray(location, indexToChange, locationElement);
    return neighbourIndex;
}

// Function push index of field to queue if it is not a wall.
// After push function mark index of field as a wall.
//  bfsQueue      - pointer to queue
//  walls         - pointer to BitArray with marked walls
//  locationIndex - index of field to be pushed to queue
static void putOneElementOnQueue(Queue *bfsQueue, BitArray *walls, size_t locationIndex) {
    if(!isWall(walls, locationIndex)) {
        push(bfsQueue, locationIndex);
        markWall(walls, locationIndex);
    }
}

// Function push to queue all indexes of fields connected to current location.
//  bfsQueue - pointer to queue
//  walls    - pointer to BitArray with marked walls
//  location - pointer to holder of array with current location coordinates
//  size     - pointer to holder of array with max coordinates
static void putOnQueue(Queue *bfsQueue, BitArray *walls, Array *location, Array *size) {
    size_t wantedSize = sizeOfArray(size);
    size_t nextLocationIndex;
    for(size_t i = 0; i < wantedSize; i++) {
        nextLocationIndex = plusNeighbour(location, size, i);
        putOneElementOnQueue(bfsQueue, walls, nextLocationIndex);
        nextLocationIndex = minusNeighbour(location, size, i);
        putOneElementOnQueue(bfsQueue, walls, nextLocationIndex);
    }
}

// Puts marker on queue
// bfsQueue - pointer to queue
// marker   - marker represented as a number to be put on queue
static void putMarker(Queue *bfsQueue, size_t marker) {
    push(bfsQueue, marker);
}

// Function checks if marker is on top of queue, which means that new layer
// of bfs will start. So way to target will be greater by 1. If queue is not
// empty will queue will mark end of new layer.
//  bfsQueue  - pointer to queue
//  marker    - number representing maker
//  wayLength - pointer to current length of way
static void readMarker(Queue *bfsQueue, size_t marker, size_t *wayLength) {
    if(first(bfsQueue) == marker) {
        pop(bfsQueue);
        if(isEmpty(bfsQueue)) {
            return;
        }
        (*wayLength)++;
        putMarker(bfsQueue, marker);
    }
}

// Function searches given labyrinth for the shortest way from start to target.
// If way exits function prints the shortest length to stdout.
// If there is no way, function prints "NO WAY" to stdout.
//  labyrinthPtr - pointer to labyrinth which have all needed information
void searchLabyrinth(Labyrinth *labyrinthPtr) {
    size_t wayLength = 0;
    Queue *bfsQueue = NULL;
    BitArray *walls = getLabyrinthWalls(labyrinthPtr);
    size_t locationIndex = getLabyrinthStart(labyrinthPtr);
    size_t targetIndex = getLabyrinthTarget(labyrinthPtr);
    Array *size = getLabyrinthSize(labyrinthPtr);
    Array *location = NULL;
    initArray(&location, sizeOfArray(size));
    initQueue(&bfsQueue);
    // No field index equal to mul of max coordinates.
    size_t lengthMarker = sizeOfBitArray(walls);

    markWall(walls, locationIndex);
    push(bfsQueue, locationIndex);
    putMarker(bfsQueue, lengthMarker);
    while(!isEmpty(bfsQueue)) {
        locationIndex = first(bfsQueue);
        pop(bfsQueue);
        if(isCompleted(targetIndex, locationIndex)) {
            printf("%zu\n", wayLength);
            freeArray(location);
            freeQueue(bfsQueue);
            return;
        }
        else {
            bitIndexToArray(locationIndex, location, size);
            putOnQueue(bfsQueue, walls, location, size);
            readMarker(bfsQueue, lengthMarker, &wayLength);
        }
    }
    freeArray(location);
    freeQueue(bfsQueue);
    printf("NO WAY\n");
}