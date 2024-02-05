#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "reading.h"
#include "arrays.h"
#include "bitarrays.h"
#include "handle-errors.h"

typedef struct RNumbers_{
    size_t a, b, m, r, s;
} RNumbers;

bool isSpaceWithoutNewLine(int asciiNumberCode) {
    return (isspace(asciiNumberCode) && asciiNumberCode != '\n');
}

static int convertToNumber(int asciiNumberCode) {
    return asciiNumberCode - '0';
}

static bool isTooBig(size_t number1, int number2, size_t max) {
    if(max / 10 >= number1 && number1 * 10 <= max - number2) {
        return false;
    }
    else {
        return true;
    }
}

static Error multiplyAndAdd(size_t *numberToMultiply, int numberToAdd, size_t max) {
    if(isTooBig(*numberToMultiply, numberToAdd, max)) {
        return INPUT_ERROR;
    }
    else {
        *numberToMultiply *= 10;
        *numberToMultiply += numberToAdd;
        return NO_ERROR;
    }
}

static Error skipZeros(int *activeChar) {
    Error errorNumber = INPUT_ERROR;
    if(*activeChar == '0') {
        errorNumber = NO_ERROR;
    }
    while(*activeChar == '0') {
        *activeChar = getchar();
    }
    return errorNumber;
}

static Error scanNumber(int *activeChar, size_t *number) {
    Error errorNumber = INPUT_ERROR;

    skipZeros(activeChar);

    while(isdigit(*activeChar)) {
        errorNumber = multiplyAndAdd(number, convertToNumber(*activeChar), SIZE_MAX);
        if(errorNumber != NO_ERROR)
            return errorNumber;
        *activeChar = getchar();
    }

    return errorNumber;
}

static Error skipSpaces(int *activeChar) {
    while(isSpaceWithoutNewLine(*activeChar)) {
        *activeChar = getchar();
    }

    if(isdigit(*activeChar) || *activeChar == '\n') {
        return NO_ERROR;
    }
    else {
        return INPUT_ERROR;
    }
}

Error readNormalLine(Array *pointerToArray) {
    size_t number;
    int activeChar = getchar();
    skipSpaces(&activeChar);
    Error errorNumber = INPUT_ERROR;

    while(activeChar != '\n') {
        number = 0;
        errorNumber = scanNumber(&activeChar, &number);
        if(errorNumber != NO_ERROR)
            return errorNumber;
        errorNumber = skipSpaces(&activeChar);
        if(errorNumber != NO_ERROR)
            return errorNumber;
        errorNumber = insertToArray(pointerToArray, number);
        if(errorNumber != NO_ERROR)
            return errorNumber;
    }

    return errorNumber;
}

static Error goodEndingOfLastLine(int *activeChar) {
    skipSpaces(activeChar);
    if(*activeChar != '\n' && *activeChar != EOF) {
        return INPUT_ERROR;
    }
    else {
        return NO_ERROR;
    }
}

static Error hexNumberToBinary(BitArray *bitArrayPtr) {
    Error errorNumber = INPUT_ERROR;

    int activeChar = getchar();
    errorNumber = skipZeros(&activeChar);
    while(isxdigit(activeChar)) {
        errorNumber = writeHex(bitArrayPtr, activeChar);
        if(errorNumber != NO_ERROR) {
            return errorNumber;
        }
        activeChar = getchar();
    }

    if(errorNumber != NO_ERROR) {
        return errorNumber;
    }

    return goodEndingOfLastLine(&activeChar);
}

static Error readHexNumber(BitArray *bitArrayPtr) {
    int activeChar = getchar();
    skipZeros(&activeChar);

    if(activeChar != 'x') {
        return INPUT_ERROR;
    }
    else {
        return hexNumberToBinary(bitArrayPtr);
    }
}

static Error scanNumber32bit(int *activeChar, size_t *number) {
    skipSpaces(activeChar);
    Error errorNumber = skipZeros(activeChar);

    while(isdigit(*activeChar)) {
        errorNumber = multiplyAndAdd(number, convertToNumber(*activeChar), UINT32_MAX);
        if(errorNumber != NO_ERROR)
            return errorNumber;
        *activeChar = getchar();
    }

    return errorNumber;
}

static Error mulAndSum(size_t a, size_t s, size_t b, size_t *result) {
    if(s != 0 && a > (SIZE_MAX - b) / s) {
        return OTHER_ERROR;
    }
    else {
        *result = a * s + b;
        return NO_ERROR;
    }
}

static Error firstModulo(RNumbers rNumbers, size_t *array) {
    Error errorNumber = NO_ERROR;
    array[0] = rNumbers.s;
    for(size_t i = 1; i <= rNumbers.r; i++) {
        errorNumber = mulAndSum(rNumbers.a, array[i - 1], rNumbers.b, &(array[i]));
        if(errorNumber != NO_ERROR) {
            return errorNumber;
        }
        array[i] %= rNumbers.m;
    }
    return NO_ERROR;
}

static Error convertRNumber(BitArray *bitArrayPtr, RNumbers rNumbers) {
    size_t *array = malloc((rNumbers.r + 1) * sizeof(size_t));
    if(array == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }
    Error errorNumber = firstModulo(rNumbers, array);
    if(errorNumber != NO_ERROR) {
        return errorNumber;
    }
    for(size_t i = 0; i <= rNumbers.r; i++) {
        array[i] %= sizeOfBitArray(bitArrayPtr);
    }
    setBitsFromRNumbers(bitArrayPtr, array, rNumbers.r);
    free(array);
    return NO_ERROR;
}

static Error readRNumber(BitArray *bitArrayPtr) {
    Error errorNumber = NO_ERROR;
    RNumbers rNumbers;
    int activeChar = getchar();

    rNumbers.a = 0;
    scanNumber32bit(&activeChar, &(rNumbers.a));
    rNumbers.b = 0;
    scanNumber32bit(&activeChar, &(rNumbers.b));
    rNumbers.m = 0;
    scanNumber32bit(&activeChar, &(rNumbers.m));
    rNumbers.r = 0;
    scanNumber32bit(&activeChar, &(rNumbers.r));
    rNumbers.s = 0;
    errorNumber = scanNumber32bit(&activeChar, &(rNumbers.s));
    if(errorNumber != NO_ERROR || rNumbers.m == 0) {
        return INPUT_ERROR;
    }

    errorNumber = convertRNumber(bitArrayPtr, rNumbers);
    if(errorNumber != NO_ERROR) {
        return errorNumber;
    }

    return goodEndingOfLastLine(&activeChar);
}

Error readFourthLine(BitArray *bitArrayPtr) {
    Error errorNumber = INPUT_ERROR;

    int activeChar = getchar();
    skipSpaces(&activeChar);

    if(activeChar == 'R') {
        errorNumber = readRNumber(bitArrayPtr);
    }
    else if(activeChar == '0') {
        errorNumber = readHexNumber(bitArrayPtr);
    }

    return errorNumber;
}

Error readFifthLine() {
    if(getchar() != EOF) {
        return INPUT_ERROR;
    }
    else {
        return NO_ERROR;
    }
}