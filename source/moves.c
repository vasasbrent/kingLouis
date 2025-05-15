#include "moves.h"
#include "pieces.h"
#include "gameState.h"

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