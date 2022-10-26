#include <stdlib.h>
#include <stdint.h>
#include "bitarrays.h"
#include "arrays.h"
#include "handle-errors.h"

static const size_t ROUND_UP = 1;
static const uint8_t ONE_BIT = 1;
static const size_t FIX = 1;
static const uint8_t BITS_IN_BYTE = 8;

struct BitArray_ {
    uint8_t *bitArray;
    size_t totalBitSize;
    size_t usedBits;
    size_t totalBytesSize;
    size_t usedBytes;
    size_t nextIndex;
    size_t fix;
};

// Function returns number of bytes which can hold given bits.
//  bitsNumber - number of bits which will be stored in bytes
static size_t bitsToBytesWithRoundUp(size_t bitsNumber) {
    size_t bytesNumber = bitsNumber/BITS_IN_BYTE;

    if(bitsNumber % BITS_IN_BYTE > 0) {
        bytesNumber += ROUND_UP;
    }

    return bytesNumber;
}

// Returns index of byte which hold given bit.
static size_t byteIndex(size_t bitIndex) {
    return bitIndex/BITS_IN_BYTE;
}


// Function initializes bit array which will hold given number of bits.
// If no memory is available function will end program.
static Error initBitArray(BitArray **bitArrayDoublePtr, size_t bitsNumber) {
    BitArray *buffer2;
    uint8_t *buffer;
    size_t neededBytes = bitsToBytesWithRoundUp(bitsNumber);

    buffer2 = malloc(sizeof(BitArray));
    if(buffer2 == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }
    else {
        *bitArrayDoublePtr = buffer2;
        (*bitArrayDoublePtr)->bitArray = NULL;
    }

    buffer = calloc(neededBytes, sizeof(uint8_t));
    if(buffer == NULL) {
        handleMemoryError(MEMORY_ERROR);
    }
    else {
        (*bitArrayDoublePtr)->bitArray = buffer;
        (*bitArrayDoublePtr)->totalBitSize = bitsNumber;
        (*bitArrayDoublePtr)->totalBytesSize = neededBytes;
        (*bitArrayDoublePtr)->usedBits = 0;
        (*bitArrayDoublePtr)->usedBytes = 0;
        (*bitArrayDoublePtr)->nextIndex = 0;
        (*bitArrayDoublePtr)->fix = neededBytes * BITS_IN_BYTE;
    }
    return NO_ERROR;
}

bool getBit(BitArray *bitArrayPtr, size_t bitIndex) {
    uint8_t *bitArray = bitArrayPtr->bitArray;
    size_t index = bitArrayPtr->totalBytesSize - 1 - byteIndex(bitIndex);
    return ONE_BIT & (bitArray[index] >> (bitIndex % BITS_IN_BYTE));
}

void setBit(BitArray *bitArrayPtr, size_t bitIndex) {
    uint8_t *bitArray = bitArrayPtr->bitArray;
    size_t index = bitArrayPtr->totalBytesSize - 1 - byteIndex(bitIndex);
    bitArray[index] ^= ONE_BIT << (bitIndex % BITS_IN_BYTE);
}

void freeBitArray(BitArray *bitArrayPtr) {
    if(bitArrayPtr != NULL) {
        free(bitArrayPtr->bitArray);
    }
    free(bitArrayPtr);
}

// Function checks if product of two numbers will greater than SIZE_MAX.
static bool isTooBig(size_t number1, size_t number2) {
    if(number1 > SIZE_MAX/number2) {
        return true;
    }
    else {
        return false;
    }
}

// Initializes a bit array for all fields in labyrinth if it is possible
//  sizes - pointer to holder of array with sizes of labyrinth
Error initMultiBitArray(BitArray **bitArrayDoublePtr, Array *sizes) {
    size_t wantedSize = 1;
    size_t size;

    for(size_t i = 0; i < sizeOfArray(sizes); i++) {
        size = elementOfArray(sizes, i);
        if(isTooBig(wantedSize, size)) {
            return OTHER_ERROR;
        }
        else {
            wantedSize *= size;
        }
    }
    return initBitArray(bitArrayDoublePtr, wantedSize);
}

// Returns a field index in bit array from given coordinates.
//  locations - pointer to holder of array with coordinates
//  sizes     - pointer to holder of array with sizes of labyrinth
size_t getBitIndex(Array *location, Array *sizes) {
    size_t dimension = sizeOfArray(sizes) - 1;
    size_t bitIndex = elementOfArray(location, dimension) - FIX;

    for(size_t i = dimension - 1; i != SIZE_MAX; i--) {
        bitIndex = bitIndex * elementOfArray(sizes, i)
                   + elementOfArray(location, i) - FIX;
    }

    return bitIndex;
}

// Transforms a field index in bit array to coordinates.
//  array - pointer to holder of array which will hold coordinates
void bitIndexToArray(size_t bitIndex, Array *array, Array *sizes) {
    size_t coordinate;
    for(size_t i = 0; i < sizeOfArray(sizes); i++) {
        coordinate = bitIndex % elementOfArray(sizes, i);
        changeArray(array, i, coordinate + FIX);
        bitIndex /= elementOfArray(sizes, i);
    }
}

size_t sizeOfBitArray(BitArray *BitArrayPtr) {
    return BitArrayPtr->totalBitSize;
}

static bool isSpaceAvailable(BitArray *bitArrayPtr) {
    return bitArrayPtr->usedBits < bitArrayPtr->totalBitSize;
}

static uint8_t convertHexToDecimal(int hexInAscii) {
    if(hexInAscii >= '0' && hexInAscii <= '9') {
        return hexInAscii - '0';
    }
    else if(hexInAscii >= 'a' && hexInAscii <= 'f') {
        return hexInAscii - 'a' + 10;
    }
    else {
        return hexInAscii - 'A' + 10;
    }
}

static void changeIndex(BitArray *bitArrayPtr) {
    if(bitArrayPtr->nextIndex == 0) {
        bitArrayPtr->nextIndex = 1;
    }
    else {
        bitArrayPtr->nextIndex = 0;
    }
}

static int convert(BitArray *bitArrayPtr, int activeChar) {
    if(bitArrayPtr->nextIndex == 0) {
        return convertHexToDecimal(activeChar) * 16;
    }
    else {
        return convertHexToDecimal(activeChar);
    }
}

static void addBits(BitArray *bitArrayPtr, int activeChar) {
    if(bitArrayPtr->usedBits > 0) {
        bitArrayPtr->usedBits += 4;
    }
    else {
        int zeroBits = 0;
        int bitToCheck = 3;
        while(!(ONE_BIT & activeChar >> bitToCheck)) {
            zeroBits++;
            bitToCheck--;
        }
        bitArrayPtr->usedBits += 4 - zeroBits;
    }
}

// Marks walls in bit array using hex number.
Error writeHex(BitArray *bitArrayPtr, int activeChar) {
    if(isSpaceAvailable(bitArrayPtr)) {
        size_t index = bitArrayPtr->usedBytes;
        addBits(bitArrayPtr, convertHexToDecimal(activeChar));
        activeChar = convert(bitArrayPtr, activeChar);
        bitArrayPtr->bitArray[index] += activeChar;
        bitArrayPtr->usedBytes += bitArrayPtr->nextIndex;
        bitArrayPtr->fix -= BITS_IN_BYTE/2;
        changeIndex(bitArrayPtr);
        if(bitArrayPtr->usedBits > bitArrayPtr->totalBitSize) {
            return INPUT_ERROR;
        }
        else {
            return NO_ERROR;
        }
    }
    else {
        return INPUT_ERROR;
    }
}

// Returns a bit with moved location.
bool getBitFixed(BitArray *bitArrayPtr, size_t bitIndex) {
    bitIndex += bitArrayPtr->fix;
    if(bitIndex < bitArrayPtr->totalBytesSize * BITS_IN_BYTE) {
        return getBit(bitArrayPtr, bitIndex);
    }
    else {
        bitIndex -= bitArrayPtr->totalBytesSize * BITS_IN_BYTE;
        return getBit(bitArrayPtr, bitIndex);
    }
}

// Changes bit with moved location.
void setBitFixed(BitArray *bitArrayPtr, size_t bitIndex) {
    bitIndex += bitArrayPtr->fix;
    if(bitIndex < bitArrayPtr->totalBytesSize * BITS_IN_BYTE) {
        setBit(bitArrayPtr, bitIndex);
    }
    else {
        bitIndex -= bitArrayPtr->totalBytesSize * BITS_IN_BYTE;
        setBit(bitArrayPtr, bitIndex);
    }
}

//Changes a bit only from 0 to 1.
void setBitFixedOneTime(BitArray *bitArrayPtr, size_t bitIndex) {
    if(bitIndex >= sizeOfBitArray(bitArrayPtr)) {
        return;
    }
    if(!getBitFixed(bitArrayPtr, bitIndex)) {
        setBitFixed(bitArrayPtr, bitIndex);
    }
}

// Marks walls form r number input.
void setBitsFromRNumbers(BitArray *bitArrayPtr, size_t *array, size_t rightIndex) {
    for(size_t i = 1; i <= rightIndex; i++) {
        setBitFixedOneTime(bitArrayPtr, array[i]);
        setBitFixedOneTime(bitArrayPtr, array[i] + UINT32_MAX + 1);
    }
}
