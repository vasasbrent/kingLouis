#ifndef UCI_H
#define UCI_H

#include <stdint.h>
#include "gameState.h"

#define INPUT_SIZE 65535
#define MAX_ARGS 100

// Result codes for command processing
typedef enum {
    PROCESS_OK = 0,
    PROCESS_ERROR_ALLOC,
    PROCESS_ERROR_ARGS,
    PROCESS_ERROR_COMMAND
} ProcessResult;

// UCI command handlers
ProcessResult processInput(const char*, size_t);
void* listenForInput(void*);
void handlePosition(char** args);
void handleGo(char** args);
void stopSearch(void);

// Position parsing helpers
void applyMoves(const char** moves, int numMoves, GameState* state);

// Search control
extern volatile int searching;

#endif