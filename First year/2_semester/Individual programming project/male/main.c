#include <stdlib.h>
#include "labyrinth.h"
#include "bfs.h"
#include "reading-combined.h"
#include "bitarrays.h"
#include "arrays.h"

int main() {
    Error errorNumber = NO_ERROR;
    // All pointers initialize to NULL so free function won't crash program.
    Array *size = NULL;
    Array *start = NULL;
    Array *target = NULL;
    BitArray *walls = NULL;
    Labyrinth *lab = NULL;
    errorNumber = readFirstLine(&size, &walls, &lab);
    if(errorNumber == NO_ERROR)
        errorNumber = readTwoLines(&start, &target, lab);
    if(errorNumber == NO_ERROR)
        errorNumber = readLastLines(walls, lab);
    if(errorNumber == NO_ERROR)
        searchLabyrinth(lab);

    freeArray(size);
    freeArray(start);
    freeArray(target);
    freeBitArray(walls);
    free(lab);
    if(errorNumber != NO_ERROR)
        return 1;
    return 0;
}