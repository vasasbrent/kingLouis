#include <stdio.h>
#include <string.h>
#include "drawBoard.h"

int main (int argc, char *argv[]) {
    char cstr[255] = DEFAULT_FEN_STRING;

    if (argc > 1) {
        memcpy(cstr, argv[1], strlen(argv[1]));
    }

    digestFEN(cstr, strlen(cstr));
    drawBoard();

    

    return 0;
}