#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "gameState.h"
#include "moves.h"
#include "pieces.h"
#include "uci.h"

void getValidMoves(GameState testGameState, int* numValidMoves, char** validMoveArr) {
    uint8_t toMoveMask = (testGameState.toMove == 'w') ? WHITE_MASK : BLACK_MASK;
    uint8_t pieceId, moveSet, moveMods;

    // Iterate over game board, generate valid moves for each piece on the "toMove" side
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (testGameState.gameBoard[i] != NO_PIECE && \
           !((testGameState.gameBoard[i] & BLACK_MASK) ^ toMoveMask)) {
            pieceId = testGameState.gameBoard[i] & PIECE_MASK;
            moveSet = validMoves[pieceId] & 0xff;
            moveMods = validMoves[pieceId] >> 8 & 0xff;
            // TODO: figure out what you're doing here
            // This is fine so far, but planning will be necessary
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
        int fromFile = move[0] - 'a';
        int fromRank = move[1] - '1';
        int toFile = move[2] - 'a';
        int toRank = move[3] - '1';

        // Validate coordinates
        if (fromFile < 0 || fromFile > 7 || fromRank < 0 || fromRank > 7 ||
            toFile < 0 || toFile > 7 || toRank < 0 || toRank > 7) {
            if (isDebug) printf("info string Move coordinates out of bounds: %s\n", move);
            continue;
        }

        int fromSquare = fromRank * 8 + fromFile;
        int toSquare = toRank * 8 + toFile;

        // Validate source square has a piece
        if (state->gameBoard[fromSquare] == NO_PIECE) {
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
            if (state->gameBoard[fromSquare] & BLACK_MASK) {
                promotionPiece |= BLACK_MASK;
            }
            state->gameBoard[toSquare] = promotionPiece;
        } else {
            state->gameBoard[toSquare] = state->gameBoard[fromSquare];
        }
        state->gameBoard[fromSquare] = NO_PIECE;

        // Update side to move
        state->toMove = (state->toMove == 'w') ? 'b' : 'w';

        // Update move counters
        if (state->toMove == 'w') {
            state->fullMoveNumber++;
        }
    }
}