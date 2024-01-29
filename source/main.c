#include <stdio.h>
#include <string.h>
#include "setup.h"
#include "drawBoard.h"

void clearInputBuffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int main (int argc, char *argv[]) {
    char cstr[255] = DEFAULT_FEN_STRING;

    if (argc > 1) {
        memcpy(cstr, argv[1], strlen(argv[1]));
    }

    digestFEN(cstr, strlen(cstr));
    drawBoard();

    char input[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    while(1) {
        scanf("%8s", &input[0]);
        printf("%s\n", &input[0]);
        for (uint8_t i = 0; i < 8; i++) {
            input[i] = ' ';
        }
        clearInputBuffer();
    }

    return 0;
}