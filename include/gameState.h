#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"

#define BOARD_SIZE 64
#define DEFAULT_FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"

typedef struct GameState {
    uint8_t       gameBoard[BOARD_SIZE];
    char          toMove;
    char          castlingAvail[4];
    uint8_t       enPassantAvail[2]; // Board spaces at which en passant is possible
    uint8_t       halfMoveClock;
    uint16_t      fullMoveNumber;
} GameState;

extern GameState globalGameState;

#endif