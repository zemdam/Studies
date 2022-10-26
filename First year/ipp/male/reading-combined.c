#include "reading-combined.h"
#include "handle-errors.h"
#include "labyrinth.h"
#include "errors.h"
#include "reading.h"
#include "arrays.h"
#include "bitarrays.h"

static const int FIRST_LINE = 1;
static const int SECOND_LINE = 2;
static const int THIRD_LINE = 3;
static const int FOURTH_LINE = 4;
static const int FIFTH_LINE = 5;

Error readFirstLine(Array **size, BitArray **walls, Labyrinth **labyrinth) {
    initLabyrinth(labyrinth);
    initArray(size, 1);
    Error errorNumber = readNormalLine(*size);
    if(errorNumber != NO_ERROR) {
        handleAllErrors(errorNumber, FIRST_LINE);
        return errorNumber;
    }
    errorNumber = initMultiBitArray(walls, *size);
    if(errorNumber != NO_ERROR) {
        handleAllErrors(errorNumber, FIRST_LINE);
        return errorNumber;
    }
    setLabyrinthSize(*labyrinth, *size);
    return NO_ERROR;
}

Error readTwoLines(Array **start, Array **target, Labyrinth *labyrinth) {
    Array *size = getLabyrinthSize(labyrinth);
    initArray(start, 1);
    initArray(target, 1);
    Error errorNumber = readNormalLine(*start);
    if(errorNumber != NO_ERROR || !isSameSize(size, *start)) {
        handleAllErrors(INPUT_ERROR, SECOND_LINE);
        return INPUT_ERROR;
    }
    errorNumber = readNormalLine(*target);
    if(errorNumber != NO_ERROR || !isSameSize(size, *target)) {
        handleAllErrors(INPUT_ERROR, THIRD_LINE);
        return INPUT_ERROR;
    }
    setLabyrinthStart(labyrinth, *start);
    setLabyrinthTarget(labyrinth, *target);
    return NO_ERROR;
}

static bool areWallsCorrect(BitArray *walls, Labyrinth *labyrinth) {
    if(getBitFixed(walls, getLabyrinthStart(labyrinth))) {
        handleAllErrors(INPUT_ERROR, SECOND_LINE);
        return false;
    }
    if(getBitFixed(walls, getLabyrinthTarget(labyrinth))) {
        handleAllErrors(INPUT_ERROR, THIRD_LINE);
        return false;
    }
    return true;
}

Error readLastLines(BitArray *walls, Labyrinth *labyrinth) {
    Error errorNumber = readFourthLine(walls);
    if(errorNumber != NO_ERROR) {
        handleAllErrors(errorNumber, FOURTH_LINE);
        return errorNumber;
    }
    if(!areWallsCorrect(walls, labyrinth)) {
        return INPUT_ERROR;
    }
    errorNumber = readFifthLine();
    if(errorNumber != NO_ERROR) {
        handleAllErrors(errorNumber, FIFTH_LINE);
        return errorNumber;
    }
    setLabyrinthWalls(labyrinth, walls);
    return NO_ERROR;
}