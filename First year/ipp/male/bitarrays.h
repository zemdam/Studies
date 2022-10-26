#ifndef IPPZAD1_BITARRAYS_H
#define IPPZAD1_BITARRAYS_H

#include "errors.h"
#include <stdbool.h>

extern Error initMultiBitArray(BitArray **BitArrayDoublePtr, Array *sizes);
extern void setBit(BitArray *BitArrayPtr, size_t bitIndex);
extern void freeBitArray(BitArray *bitArrayPtr);
extern size_t sizeOfBitArray(BitArray *BitArrayPtr);
extern Error writeHex(BitArray *bitArrayPtr, int activeChar);
extern bool getBit(BitArray *BitArrayPtr, size_t bitIndex);
extern bool getBitFixed(BitArray *bitArrayPtr, size_t bitIndex);
extern void setBitFixed(BitArray *bitArrayPtr, size_t bitIndex);
extern size_t getBitIndex(Array *location, Array *sizes);
extern void bitIndexToArray(size_t bitIndex, Array *array, Array *sizes);
extern void setBitsFromRNumbers(BitArray *bitArrayPtr, size_t *array, size_t rightIndex);

#endif //IPPZAD1_BITARRAYS_H
