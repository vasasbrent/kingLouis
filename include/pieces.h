#define BLACK_MASK 0b1000
#define WHITE_MASK 0b0000

#define NO_PIECE   0b0000
#define PAWN       0b0001
#define KNIGHT     0b0010
#define BISHOP     0b0011
#define ROOK       0b0100
#define QUEEN      0b0101
#define KING       0b0110
#define NUM_PIECES 0b0111

// Least Significant Byte == Squares to which piece can be moved
// Most Significant Byte:
// 1: Alt move set, i.e. knight
// 2: Flip for black
// 3: Castling
// 4: pawn special double move
// 5: continuous movement
#define MOD_MOVEMENT_SET  1
#define FLIP_FOR_BLACK    2
#define CASTLE_POSSIBLE   3
#define PAWN_FIRST_DOUBLE 4
#define CONTINUOUS_MOVE   5

int8_t standardMoveSet[8] = {7, 8, 9, 1, -7, -8, -9, -1};
int8_t modifiedMoveSet[8] = {15, 17, 10, -6, -15, -17, -10, 6};

uint16_t validMoves[NUM_PIECES] = {0x0000, //0b0000000000000000 no piece
                                   0x0A07, //0b0000101000000111 pawn
                                   0x01ff, //0b0000000111111111 knight
                                   0x1055, //0b0001000001010101 bishop
                                   0x10aa, //0b0001000010101010 rook
                                   0x10ff, //0b0001000011111111 queen
                                   0x03ff};//0b0000000011111111 king

char pieceIDs[NUM_PIECES * 2] = {' ',
                                 'p',
                                 'n',
                                 'b',
                                 'r',
                                 'q',
                                 'k',
                                 ' ',
                                 'P',
                                 'N',
                                 'B',
                                 'R',
                                 'Q',
                                 'K',}
