#include <stdio.h>
#include <string.h>
#include "uci.h"
#include "version.h"

static const char* uciString = "uci";

void clearInputBuffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// Land here if any input is received by the kingLouis, reply with "meep"?
void processInput(const char* inputBuffer, uint8_t inputSize) {
    // TODO: Make this robust to arbitrary whitespace, it's dumb now in service of getting something here, be better.
    if (!strcmp(&inputBuffer[0], &uciString[0])) {
        printf("id name %s %s\n", getProgramName(), getVersionNumber());
        printf("id author %s\n", getAuthor());
        printf("uciok\n");
    }
}

void* listenForInput(void* arg) {
    char input[INPUT_SIZE];
    while(1) {
        scanf("%255s", &input[0]);
        processInput(&input[0], strlen(input));
    
        for (uint8_t i = 0; i < 8; i++) {
            input[i] = ' ';
        }
        clearInputBuffer();
    }
}
