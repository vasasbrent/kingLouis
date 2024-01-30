#include <stdio.h>
#include <string.h>
#include "uci.h"
#include "version.h"

static const char* uciString = "uci";

// Land here if any input is recieved by the kingLouis, reply with "meep"?
void processInput(const char* inputBuffer, uint8_t inputSize) {
    // TODO: Make this robust to arbitrary whitespace, it's dumb now in service of getting something here, be better.
    if (!strcmp(&inputBuffer[0], &uciString[0])) {
        printf("id name %s %s\n", getProgramName(), getVersionNumber());
        printf("id author %s\n", getAuthor());
    }
}