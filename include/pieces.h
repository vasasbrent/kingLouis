#ifndef PIECES_H
#define PIECES_H

#include "types.h"

#define BLACK_MASK 0b1000
#define WHITE_MASK 0b0000

#define NO_PIECE   0b0000
#define PAWN       0b0001
#define KNIGHT     0b0010
#define BISHOP     0b0011
#define ROOK       0b0100
#define QUEEN      0b0101
#define KING       0b0110
#define NUM_PIECES 0b0111

// Least Significant Byte == Squares to which piece can be moved
// Most Significant Byte:
// 1: Alt move set, i.e. knight
// 2: Flip for black
// 3: Castling
// 4: pawn special double move
// 5: continuous movement
#define MOD_MOVEMENT_SET  1
#define FLIP_FOR_BLACK    2
#define CASTLE_POSSIBLE   3
#define PAWN_FIRST_DOUBLE 4
#define CONTINUOUS_MOVE   5

extern int8_t standardMoveSet[8];
extern int8_t modifiedMoveSet[8];

extern uint16_t validMoves[NUM_PIECES];

extern char pieceIDs[NUM_PIECES * 2 + 1];

#endif