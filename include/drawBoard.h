#include <stdint.h>

#define DEFAULT_FEN_STRING "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"

void drawBoard(void);
void digestFEN(char*, size_t);
uint64_t getValidMoves(uint8_t);