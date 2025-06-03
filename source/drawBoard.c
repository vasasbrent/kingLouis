#include <stdio.h>
#include <stdlib.h>
#include "drawBoard.h"
#include "gameState.h"
#include "pieces.h"

//GameState globalGameState;

void drawBoard(GameState globalGameState) {
    for (uint8_t row = 8; row > 0; row--) {
        printf("   ---------------------------------\n %c |", (row + ATOI_OFFSET));
        for (uint8_t col = 0; col < 8; col++) {
            printf(" %c |", pieceIDs[globalGameState.gameBoard[(row - 1) * 8 + col]]);
        }
        printf("\n");
    }
    printf("   ---------------------------------\n     a   b   c   d   e   f   g   h\n");
}
