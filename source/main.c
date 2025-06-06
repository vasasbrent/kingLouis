#include <time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "setup.h"
#include "drawBoard.h"
#include "uci.h"
#include "gameState.h"

GameState globalGameState;

int main (int argc, char *argv[]) {
    // Default to start position
    // Different positions will be handled by input parser
    char cstr[255] = DEFAULT_FEN_STRING;

    if (argc > 1) {
        printf("Command line arguments not supported yet\n");
        return -1;
    }

    globalGameState = digestFEN(cstr, strlen(cstr));
    drawBoard(globalGameState);

    // Initialize POSIX thread for UCI
    pthread_t threadUci;
    pthread_create(&threadUci, NULL, listenForInput, NULL);

    while(1) {
        //printf(".");
        sleep(1);
    }

    return 0;
}