#include <stdlib.h>
#include "setup.h"
#include "pieces.h"

uint8_t getSquareFromAlgebraic(const char* algSquare) {
    uint8_t col = 0, row = 0;
    char colOffset = algSquare[0] - ATOI_OFFSET;

    col = atoi(&colOffset) - 1;
    row = atoi(&algSquare[1]) - 1;

    return (row * 8 + col);
}

GameState digestFEN(char* FENString, size_t length) {
    GameState localGameState;
    uint8_t col = 0, row = 7;
    uint8_t fenIndex = 0;

    uint8_t tempIndex = 0;
    char tempStr[4] = {' ', ' ', ' ', ' '};

    // Piece positioning loop
    while (FENString[fenIndex] != ' ') {
        if (FENString[fenIndex] == '/') {
            row--; col = 0;
        } else if (FENString[fenIndex] > '0' && FENString[fenIndex] < '9') {
            for (uint8_t i = 0; i < atoi(&FENString[fenIndex]); i++) {
                localGameState.gameBoard[row * 8 + col] = NO_PIECE;
                col++;
            }
        } else {
            // Handle piece type
            if (FENString[fenIndex] == 'p' || FENString[fenIndex] == 'P')
                localGameState.gameBoard[row * 8 + col] = PAWN;
            else if (FENString[fenIndex] == 'n' || FENString[fenIndex] == 'N')
                localGameState.gameBoard[row * 8 + col] = KNIGHT;
            else if (FENString[fenIndex] == 'b' || FENString[fenIndex] == 'B')
                localGameState.gameBoard[row * 8 + col] = BISHOP;
            else if (FENString[fenIndex] == 'r' || FENString[fenIndex] == 'R')
                localGameState.gameBoard[row * 8 + col] = ROOK;
            else if (FENString[fenIndex] == 'q' || FENString[fenIndex] == 'Q')
                localGameState.gameBoard[row * 8 + col] = QUEEN;
            else if (FENString[fenIndex] == 'k' || FENString[fenIndex] == 'K')
                localGameState.gameBoard[row * 8 + col] = KING;
            else
                localGameState.gameBoard[row * 8 + col] = NO_PIECE;

            // Handle piece color
            if (FENString[fenIndex] >= 'a' && FENString[fenIndex] <= 'z')
                localGameState.gameBoard[row * 8 + col] |= BLACK_MASK;
            col++;
        }
        fenIndex++;
    }
    fenIndex++; // skip delimiting space

    // Populate To Move
    localGameState.toMove = FENString[fenIndex];
    fenIndex += 2;

    // Castling availability
    tempIndex = 0;
    while (FENString[fenIndex] != ' ') {
        localGameState.castlingAvail[tempIndex] = FENString[fenIndex];
        fenIndex++, tempIndex++;
    }
    fenIndex++; // skip delimiting space

    // Handle the brutal machinations of the 19th century French
    tempIndex = 0;
    localGameState.enPassantAvail[0] = localGameState.enPassantAvail[1] = 0;
    if (FENString[fenIndex] != '-') {
        while (FENString[fenIndex] != ' ') {
            localGameState.enPassantAvail[tempIndex] = getSquareFromAlgebraic(&FENString[fenIndex]);
            fenIndex += 2;
        }
    } else {
        fenIndex++;
    }
    fenIndex++;

    // Half move clock
    tempIndex = 0;
    while (FENString[fenIndex] != ' ') {
        tempStr[tempIndex] = FENString[fenIndex];
        fenIndex++, tempIndex++;
    }
    localGameState.halfMoveClock = atoi(tempStr);
    fenIndex++;

    // Total move number
    tempIndex = 0;
    while (FENString[fenIndex] != '\0') {
        tempStr[tempIndex] = FENString[fenIndex];
        fenIndex++, tempIndex++;
    }
    localGameState.fullMoveNumber = atoi(tempStr);

    return localGameState;
}
