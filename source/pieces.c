#include "pieces.h"

int8_t standardMoveSet[8] = {7, 8, 9, 1, -7, -8, -9, -1};
int8_t modifiedMoveSet[8] = {15, 17, 10, -6, -15, -17, -10, 6};

float pieceValue[NUM_PIECES] = {0,
                                1.0f,
                                }

uint16_t validMoves[NUM_PIECES] = {0x0000, //0b0000000000000000 no piece
                                   0x0A07, //0b0000101000000111 pawn
                                   0x01ff, //0b0000000111111111 knight
                                   0x1055, //0b0001000001010101 bishop
                                   0x10aa, //0b0001000010101010 rook
                                   0x10ff, //0b0001000011111111 queen
                                   0x03ff};//0b0000000011111111 king

char pieceIDs[NUM_PIECES * 2 + 1] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K', ' ', ' ', 'p', 'n', 'b', 'r', 'q', 'k'};
