#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"

#define BOARD_SIDE 8
#define DEFAULT_FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"

typedef struct BoardSquare {
    uint8_t rank;
    uint8_t file;
} BoardSquare;

typedef struct GameState {
    uint8_t       gameBoard[BOARD_SIDE][BOARD_SIDE];  // [rank][file], [0][0] is a8, [7][7] is h1
    char          toMove;
    char          castlingAvail[4];
    BoardSquare   enPassantSquare;
    uint8_t       halfMoveClock;
    uint16_t      fullMoveNumber;
} GameState;

extern GameState globalGameState;

// Convert between 1D and 2D coordinates
static inline void get2DCoords(int square1D, int* rank, int* file) {
    *rank = square1D / 8;
    *file = square1D % 8;
}

static inline int get1DCoord(int rank, int file) {
    return rank * 8 + file;
}

static inline BoardSquare getBoardSquare(char* algSquare) {
    BoardSquare square;
    square.rank = '8' - algSquare[1];
    square.file = algSquare[0] - 'a';
    return square;
}

#endif