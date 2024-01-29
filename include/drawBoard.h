#ifndef DRAW_BOARD_H
#define DRAW_BOARD_H

#include "types.h"

#define DEFAULT_FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"

void drawBoard(void);
uint64_t getValidMoves(uint8_t);

#endif