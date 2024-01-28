#define BOARD_SIZE = 64

struct GameState {
    uint8_t       gameBoard[BOARD_SIZE];
    char          toMove;
    char          castlingAvail[4];
    uint8_t       enPassantAvail[2]; // Board spaces at which en passant is possible
    uint8_t       halfMoveClock;
    uint16_t      fullMoveNumber;
};

typedef struct GameState GameState;
