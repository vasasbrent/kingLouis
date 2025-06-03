#ifndef UCI_H
#define UCI_H

#include <stdint.h>
#include <stdbool.h>
#include "gameState.h"

#define INPUT_SIZE 65535
#define MAX_ARGS 100
#define MAX_ARG_LENGTH 256

void processInput(const char*, size_t);
void* listenForInput(void*);
void handlePosition(char args[MAX_ARGS][MAX_ARG_LENGTH]);
void handleGo(char args[MAX_ARGS][MAX_ARG_LENGTH]);
void stopSearch(void);

extern volatile int searching;
extern volatile bool isDebug;

#endif