#include <stdio.h>
#include <stdlib.h>
#include "drawBoard.h"
#include "gameState.h"
#include "pieces.h"

//GameState globalGameState;

void drawBoard(GameState globalGameState) {
    for (uint8_t rank = 0; rank < BOARD_SIDE; rank++) {
        printf("   ---------------------------------\n %d |", 8 - rank);
        for (uint8_t file = 0; file < BOARD_SIDE; file++) {
            printf(" %c |", pieceIDs[globalGameState.gameBoard[rank][file]]);
        }
        printf("\n");
    }
    printf("   ---------------------------------\n     a   b   c   d   e   f   g   h\n");
}
