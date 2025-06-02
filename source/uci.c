#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uci.h"
#include "version.h"

// CONSTANTS
// "debug" command constants
#define DEBUG_SWITCH_ARG_IDX 1
#define DEBUG_ON "on"
#define DEBUG_OFF "off"

static const char* commands[] = \
    {"uci",
     "debug",
     "isready"};

enum input_strings {
    UCI = 0,
    DEBUG,
    IS_READY,
    INPUT_STRINGS_LEN
};

void clearInputBuffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void argParse(const char* inputBuffer, char** args) {
    uint8_t argNum = 0, inArg = 0, argChar = 0, i = 0;

    while (inputBuffer[i] != '\0') {
        if (inArg) {
            if (inputBuffer[i] == ' ') {
                args[argNum][argChar] = '\0';
                inArg = argChar = 0;
                argNum++;
            } else {
                args[argNum][argChar] = inputBuffer[i];
                argChar++;
            }
        } else {
            if (inputBuffer[i] != ' ') {
                inArg = 1;
                args[argNum][argChar] = inputBuffer[i];
                argChar++;
            }
        }

        i++;
    }

    args[argNum][argChar] = '\0';
}

void processInput(const char* inputBuffer, uint8_t inputSize) {
    char** args = (char**)malloc(MAX_ARGS * sizeof(char*));
    for (uint8_t i = 0; i < MAX_ARGS; i++) {
        args[i] = (char*)malloc(INPUT_SIZE * sizeof(char));
    }
    
    argParse(inputBuffer, args);

    uint8_t inputIndex = 0;

    for (inputIndex; inputIndex < INPUT_STRINGS_LEN; inputIndex++) {
        if (!strcmp(args[0], commands[inputIndex]))
            break;
    }

    switch (inputIndex) {
        case UCI:
            printf("id name %s %s\n", getProgramName(), getVersionNumber());
            printf("id author %s\n", getAuthor());
            printf("uciok\n");
            break;

        // TODO (ongoing): Make debug mode, toggle switch here
        case DEBUG:
            if (!strcmp(args[DEBUG_SWITCH_ARG_IDX], DEBUG_ON))
                printf("debug turned on\n");
            else if (!strcmp(args[DEBUG_SWITCH_ARG_IDX], DEBUG_OFF))
                printf("debug turned off\n");
            else
                printf("received malformed debug arg: %s\n", args[DEBUG_SWITCH_ARG_IDX]);
            break;

        case IS_READY:
            printf("readyok\n");
            break;

        default:
            printf("MISSED ALL CASES!\nCommand: %s\nInput Buffer: %s\nString Length: %d\n", args[0], inputBuffer, inputSize);
            break;
    }

    for (uint8_t i = 0; i < MAX_ARGS; i++) {
        free(args[i]);
    }
    free(args);
}

void* listenForInput(void* arg) {
    char input[INPUT_SIZE];
    while(1) {
        scanf("%[^\n]255s", &input[0]);
        processInput(input, strlen(input));
    
        for (uint8_t i = 0; i < 8; i++) {
            input[i] = ' ';
        }
        clearInputBuffer();
    }
}
