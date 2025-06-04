#ifndef MOVES_H
#define MOVES_H

#include "gameState.h"

#define MAX_SLIDE_DISTANCE 8

void getValidMoves(GameState, int*, char**);
void applyMoves(const char**, int, GameState*);

#endif