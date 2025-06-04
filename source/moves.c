#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "gameState.h"
#include "moves.h"
#include "pieces.h"
#include "uci.h"

bool isMoveOffBoard(int file, int rank) {
    return file < 0 || file > 7 || rank < 0 || rank > 7;
}

bool isSquareOccupiedByFriendlyPiece(uint8_t piece, uint8_t toMoveMask) {
    return piece != NO_PIECE && !((piece & BLACK_MASK) ^ toMoveMask);
}

bool isSquareOccupiedByEnemyPiece(uint8_t piece, uint8_t toMoveMask) {
    return piece != NO_PIECE && ((piece & BLACK_MASK) ^ toMoveMask);
}

void getValidMoves(GameState testGameState, int* numValidMoves, char** validMoveArr) {
    uint8_t toMoveMask = (testGameState.toMove == 'w') ? WHITE_MASK : BLACK_MASK;
    uint8_t pieceId, moveSet, moveMods;
    char moveBuffer[5];  // Max length of a move string (e.g., "e2e4\0" or "e7e8q\0")
    *numValidMoves = 0;

    // Iterate over game board, generate valid moves for each piece on the "toMove" side
    for (int rank = 0; rank < BOARD_SIDE; rank++) {
        for (int file = 0; file < BOARD_SIDE; file++) {
            if (testGameState.gameBoard[rank][file] != NO_PIECE && \
               !((testGameState.gameBoard[rank][file] & BLACK_MASK) ^ toMoveMask)) {
                pieceId = testGameState.gameBoard[rank][file] & PIECE_MASK;
                moveSet = validMoves[pieceId] & 0xff;
                moveMods = validMoves[pieceId] >> 8 & 0xff;
                // TODO: figure out what you're doing here
                // This is fine so far, but planning will be necessary
                for (int move = 0; move < 8; move++) {
                    while (moveSet << move & 0b1) {
                        // TODO: work this out once gameboard is 2d array, 1d was a bad idea
                        break;
                    }
                }
            }
        }
    }
}

void applyMoves(const char** moves, int numMoves, GameState* state) {
    if (!moves || !state || numMoves <= 0) {
        return;
    }

    for (int i = 0; i < numMoves; i++) {
        const char* move = moves[i];
        if (!move || strlen(move) < 4) {
            if (isDebug) printf("info string Invalid move format: %s\n", move ? move : "null");
            continue;
        }

        // Convert algebraic notation to board coordinates
        // TODO: this is a hack, we need to use the getBoardSquare function
        int fromFile = move[0] - 'a';
        int fromRank = '8' - move[1];  // Convert to 0-7 rank, 0 is top rank
        int toFile = move[2] - 'a';
        int toRank = '8' - move[3];    // Convert to 0-7 rank, 0 is top rank

        // Validate coordinates
        if (isMoveOffBoard(fromFile, fromRank) || isMoveOffBoard(toFile, toRank)) {
            if (isDebug) printf("info string Move coordinates out of bounds: %s\n", move);
            continue;
        }

        // Validate source square has a piece
        if (state->gameBoard[fromRank][fromFile] == NO_PIECE) {
            if (isDebug) printf("info string No piece at source square: %s\n", move);
            continue;
        }

        // Handle promotion
        if (strlen(move) == 5) {
            uint8_t promotionPiece = NO_PIECE;
            switch(move[4]) {
                case 'q': promotionPiece = QUEEN; break;
                case 'r': promotionPiece = ROOK; break;
                case 'b': promotionPiece = BISHOP; break;
                case 'n': promotionPiece = KNIGHT; break;
                default:
                    if (isDebug) printf("info string Invalid promotion piece: %c\n", move[4]);
                    continue;
            }
            if (state->gameBoard[fromRank][fromFile] & BLACK_MASK) {
                promotionPiece |= BLACK_MASK;
            }
            state->gameBoard[toRank][toFile] = promotionPiece;
        } else {
            state->gameBoard[toRank][toFile] = state->gameBoard[fromRank][fromFile];
        }
        state->gameBoard[fromRank][fromFile] = NO_PIECE;

        // Update side to move
        state->toMove = (state->toMove == 'w') ? 'b' : 'w';

        // Update move counters
        if (state->toMove == 'w') {
            state->fullMoveNumber++;
        }
    }
}