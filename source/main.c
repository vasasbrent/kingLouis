#include <time.h>
#include <stdio.h>
#include <string.h>
#include "setup.h"
#include "drawBoard.h"
#include "uci.h"

#define DEFAULT_FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"
#define INPUT_SIZE 255

void clearInputBuffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int main (int argc, char *argv[]) {
    char cstr[INPUT_SIZE] = DEFAULT_FEN_STRING;
    char input[INPUT_SIZE];

    if (argc > 1) {
        memcpy(cstr, argv[1], strlen(argv[1]));
    }

    digestFEN(cstr, strlen(cstr));
    drawBoard();

    while(1) {
        // TODO: Un-hardcode this, should be possible, you're just dumb
        scanf("%255s", &input[0]);
        processInput(&input[0], strlen(input));

        for (uint8_t i = 0; i < 8; i++) {
            input[i] = ' ';
        }
        clearInputBuffer();
    }

    return 0;
}