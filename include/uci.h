#ifndef UCI_H
#define UCI_H

#include <stdint.h>

#define INPUT_SIZE 255
#define MAX_ARGS 10

void processInput(const char*, uint8_t);
void* listenForInput(void*);

#endif