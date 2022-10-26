#ifndef IPPZAD1_READING_COMBINED_H
#define IPPZAD1_READING_COMBINED_H

#include "errors.h"

extern Error readLastLines(BitArray *walls, Labyrinth *labyrinth);
extern Error readTwoLines(Array **start, Array **target, Labyrinth *labyrinth);
extern Error readFirstLine(Array **size, BitArray **walls, Labyrinth **labyrinth);

#endif //IPPZAD1_READING_COMBINED_H
