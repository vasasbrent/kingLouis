#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "gameState.h"
#include "setup.h"
#include "pieces.h"

GameState digestFEN(char* FENString, size_t length) {
    GameState localGameState;
    uint8_t rank = 0, file = 0;  // Start from top-left (a8)
    uint8_t fenIndex = 0;

    uint8_t tempIndex = 0;
    char tempStr[4] = {' ', ' ', ' ', ' '};

    // Initialize board to empty
    for (int r = 0; r < BOARD_SIDE; r++) {
        for (int f = 0; f < BOARD_SIDE; f++) {
            localGameState.gameBoard[r][f] = NO_PIECE;
        }
    }

    // Piece positioning loop
    while (FENString[fenIndex] != ' ') {
        if (FENString[fenIndex] == '/') {
            rank++; file = 0;
        } else if (FENString[fenIndex] >= '1' && FENString[fenIndex] <= '8') {
            file += FENString[fenIndex] - '0';
        } else {
            uint8_t piece = NO_PIECE;
            
            // Handle piece type
            switch(FENString[fenIndex]) {
                case 'p': case 'P': piece = PAWN; break;
                case 'n': case 'N': piece = KNIGHT; break;
                case 'b': case 'B': piece = BISHOP; break;
                case 'r': case 'R': piece = ROOK; break;
                case 'q': case 'Q': piece = QUEEN; break;
                case 'k': case 'K': piece = KING; break;
            }

            // Handle piece color
            if (FENString[fenIndex] >= 'a' && FENString[fenIndex] <= 'z') {
                piece |= BLACK_MASK;
            }

            localGameState.gameBoard[rank][file] = piece;
            file++;
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

    // Handle en passant
    tempIndex = 0;
    localGameState.enPassantSquare.rank = localGameState.enPassantSquare.rank = -1;
    if (FENString[fenIndex] != '-') {
        while (FENString[fenIndex] != ' ') {
            localGameState.enPassantSquare = getBoardSquare(&FENString[fenIndex]);
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
