#ifndef UCI_H
#define UCI_H

#include <stdint.h>

#define INPUT_SIZE 65535
#define MAX_ARGS 100

void processInput(const char*, uint8_t);
void* listenForInput(void*);

#endif