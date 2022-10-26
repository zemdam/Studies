#ifndef IPPZAD1_LABYRINTH_H
#define IPPZAD1_LABYRINTH_H

#include "errors.h"
#include <stddef.h>

extern Error initLabyrinth(Labyrinth **labyrinthDoublePtr);
extern void setLabyrinthSize(Labyrinth *labyrinthPtr, Array *size);
extern void setLabyrinthStart(Labyrinth *labyrinthPtr, Array *start);
extern void setLabyrinthTarget(Labyrinth *labyrinthPtr, Array *target);
extern void setLabyrinthWalls(Labyrinth *labyrinthPtr, BitArray *walls);
extern Array *getLabyrinthSize(Labyrinth *labyrinthPtr);
extern size_t getLabyrinthStart(Labyrinth *labyrinthPtr);
extern size_t getLabyrinthTarget(Labyrinth *labyrinthPtr);
extern BitArray *getLabyrinthWalls(Labyrinth *labyrinthPtr);

#endif //IPPZAD1_LABYRINTH_H
