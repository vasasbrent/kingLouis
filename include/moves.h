#ifndef MOVES_H
#define MOVES_H

#include "gameState.h"

void getValidMoves(GameState, int*, char**);
void applyMoves(const char**, int, GameState*);

#endif