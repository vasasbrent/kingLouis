#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drawBoard.h"
#include "gameState.h"
#include "pieces.h"

#define ATOI_OFFSET 48

GameState globalGameState;

uint8_t getSquareFromAlgebraic(const char* algSquare) {
    uint8_t col = 0, row = 0;
    char colOffset = algSquare[0] - ATOI_OFFSET;

    col = atoi(&colOffset) - 1;
    row = atoi(&algSquare[1]) - 1;

    return (row * 8 + col);
}

char* getFENRow(char* FENString, uint8_t desiredRow, size_t length) {
    uint8_t stringIndex = 0, slashCounter = 0, retIndex = 0;
    char FENRow[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

    // Find desired section
    while (stringIndex < length) {
        if(slashCounter == desiredRow)
            break;
        if (FENString[stringIndex] == '/')
            slashCounter++;
        stringIndex++;
    }

    // Populate row
    while (stringIndex < length) {
        if (FENString[stringIndex] > '0' && FENString[stringIndex] < '9') {
            for (uint8_t i = 0; i < atoi(&FENString[stringIndex]); i++){
                FENRow[retIndex] = ' ';
                retIndex++;
            }
        } else if (FENString[stringIndex] != '/' && FENString[stringIndex] != ' ') {
            FENRow[retIndex] = FENString[stringIndex];
            retIndex++;
        } else {
            break;
        }
        stringIndex++;
    }

    // Return desired string
    char* retRow = &FENRow[0];
    return retRow;
}

char getPieceAscii(uint8_t pieceID) {
    
}

void drawBoard() {
    for (uint8_t row = 7; row >= 0; row--) {
        printf("---------------------------------\n|");
        for (uint8_t col = 0; col < 8; col++) {
            printf(" %c |", pieceIDs[globalGameState.gameBoard[row * 8 + col]]);
        }
        printf("\n");
    }
    printf("---------------------------------\n");
}

void digestFEN(char* FENString, size_t length) {
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
                globalGameState.gameBoard[row * 8 + col] = NO_PIECE;
                col++;
            }
        } else {
            //sorry
            globalGameState.gameBoard[row * 8 + col] =\
                (((FENString[fenIndex] >= 'a' && FENString[fenIndex] <= 'z') ? BLACK_MASK : WHITE_MASK) |\
                (((((((FENString[fenIndex] == 'p' || FENString[fenIndex] == 'P') ? PAWN :\
                ((((((FENString[fenIndex] == 'n' || FENString[fenIndex] == 'N') ? KNIGHT :\
                (((((FENString[fenIndex] == 'b' || FENString[fenIndex] == 'B') ? BISHOP :\
                ((((FENString[fenIndex] == 'r' || FENString[fenIndex] == 'R') ? ROOK :\
                (((FENString[fenIndex] == 'q' || FENString[fenIndex] == 'Q') ? QUEEN :\
                ((FENString[fenIndex] == 'k' || FENString[fenIndex] == 'K') ? KING :\
                0))))))))))))))))))))));
            col++;
        }
        fenIndex++;
    }
    fenIndex++; // skip delimiting space

    // Populate To Move
    globalGameState.toMove = FENString[fenIndex];
    fenIndex += 2;

    // Castling availability
    tempIndex = 0;
    while (FENString[fenIndex] != ' ') {
        globalGameState.castlingAvail[tempIndex] = FENString[fenIndex];
        fenIndex++, tempIndex++;
    }
    fenIndex++; // skip delimiting space

    // Handle the brutal machinations of the 19th century French
    tempIndex = 0;
    globalGameState.enPassantAvail[0] = globalGameState.enPassantAvail[1] = 0;
    if (FENString[fenIndex] != '-') {
        while (FENString[fenIndex] != ' ') {
            globalGameState.enPassantAvail[tempIndex] = getSquareFromAlgebraic(&FENString[fenIndex]);
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
    globalGameState.halfMoveClock = atoi(tempStr);
    fenIndex++;

    // Total move number
    tempIndex = 0;
    while (FENString[fenIndex] != '\0') {
        tempStr[tempIndex] = FENString[fenIndex];
        fenIndex++, tempIndex++;
    }
    globalGameState.fullMoveNumber = atoi(tempStr);
}

uint64_t getValidMoves(uint8_t position) {
    uint64_t validMoves = 0;

    // useless to strip identifier, must be used
    //uint8_t piece = globalGameState.gameBoard[position] & ~BLACK_MASK;
    //printf("%d ", piece);

    return validMoves;
}
