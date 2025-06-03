#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "uci.h"
#include "version.h"
#include "setup.h"
#include "pieces.h"
#include "gameState.h"

// CONSTANTS
// "debug" command constants
#define DEBUG_SWITCH_ARG_IDX 1
#define DEBUG_ON "on"
#define DEBUG_OFF "off"

// Longest reasonable command would be "position fen" followed by longest possible FEN
// plus a long sequence of moves
#define INPUT_BUFFER_SIZE 8192
#define MAX_ARGS 100
#define MAX_ARG_LENGTH 256

volatile int searching = 0;
volatile bool pondering = false;
volatile bool isDebug = false;

// Structure to hold search parameters
typedef struct SearchParams {
    int wtime;          // White's remaining time in ms
    int btime;          // Black's remaining time in ms
    int winc;           // White's increment per move in ms
    int binc;           // Black's increment per move in ms
    int movestogo;      // Moves until next time control
    int depth;          // Maximum search depth
    int nodes;          // Maximum nodes to search
    int mate;           // Search for mate in x moves
    int movetime;       // Time to search in ms
    bool infinite;      // Search until stopped
    bool ponder;        // Search in ponder mode
    const char** searchmoves; // Restrict search to these moves
    int num_searchmoves;
} SearchParams;

static const char* commands[] = \
    {"uci",
     "debug",
     "isready",
     "registration",
     "position",
     "go",
     "stop",
     "ponderhit",
     "quit"};

enum input_strings {
    UCI = 0,
    DEBUG,
    IS_READY,
    REGISTRATION,
    POSITION,
    GO,
    STOP,
    PONDERHIT,
    QUIT,
    INPUT_STRINGS_LEN
};

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}


void applyMoves(const char** moves, int numMoves, GameState* state) {
    if (!moves || !state || numMoves <= 0) {
        return;
    }

    for (int i = 0; i < numMoves; i++) {
        const char* move = moves[i];
        if (!move || strlen(move) < 4) {  // Minimum move length is 4 (e.g., "e2e4")
            if (isDebug) printf("info string Invalid move format: %s\n", move ? move : "null");
            continue;
        }

        // Convert algebraic notation to board coordinates
        int fromFile = move[0] - 'a';
        int fromRank = move[1] - '1';
        int toFile = move[2] - 'a';
        int toRank = move[3] - '1';

        // Validate coordinates
        if (fromFile < 0 || fromFile > 7 || fromRank < 0 || fromRank > 7 ||
            toFile < 0 || toFile > 7 || toRank < 0 || toRank > 7) {
            if (isDebug) printf("info string Move coordinates out of bounds: %s\n", move);
            continue;
        }

        int fromSquare = fromRank * 8 + fromFile;
        int toSquare = toRank * 8 + toFile;

        // Validate source square has a piece
        if (state->gameBoard[fromSquare] == NO_PIECE) {
            if (isDebug) printf("info string No piece at source square: %s\n", move);
            continue;
        }

        // Handle promotion
        if (strlen(move) == 5) {
            uint8_t promotionPiece = NO_PIECE;
            switch(move[4]) {
                case 'q': promotionPiece = QUEEN; break;
                case 'r': promotionPiece = ROOK; break;
                case 'b': promotionPiece = BISHOP; break;
                case 'n': promotionPiece = KNIGHT; break;
                default:
                    if (isDebug) printf("info string Invalid promotion piece: %c\n", move[4]);
                    continue;
            }
            if (state->gameBoard[fromSquare] & BLACK_MASK) {
                promotionPiece |= BLACK_MASK;
            }
            state->gameBoard[toSquare] = promotionPiece;
        } else {
            state->gameBoard[toSquare] = state->gameBoard[fromSquare];
        }
        state->gameBoard[fromSquare] = NO_PIECE;

        // Update side to move
        state->toMove = (state->toMove == 'w') ? 'b' : 'w';

        // Update move counters
        if (state->toMove == 'w') {
            state->fullMoveNumber++;
        }
    }
}

void handlePosition(char** args) {
    int argIdx = 1;
    char fen[100] = {0};
    
    if (!args || !args[argIdx]) {
        if (isDebug) printf("info string Invalid position command\n");
        return;
    }

    if (!strcmp(args[argIdx], "startpos")) {
        strcpy(fen, DEFAULT_FEN_STRING);
        argIdx++;
    } else if (!strcmp(args[argIdx], "fen")) {
        argIdx++;
        // Concatenate FEN parts until "moves" or end
        while (args[argIdx] && strcmp(args[argIdx], "moves")) {
            strcat(fen, args[argIdx]);
            strcat(fen, " ");
            argIdx++;
        }
    } else {
        if (isDebug) printf("info string Expected 'startpos' or 'fen', got: %s\n", args[argIdx]);
        return;
    }
    
    // Parse FEN using existing function
    globalGameState = digestFEN(fen, strlen(fen));
    
    // Handle moves if present
    if (args[argIdx] && !strcmp(args[argIdx], "moves")) {
        argIdx++;
        if (!args[argIdx]) {
            if (isDebug) printf("info string 'moves' specified but no moves given\n");
            return;
        }

        // Count moves first
        int numMoves = 0;
        int moveStart = argIdx;
        while (args[argIdx] && strlen(args[argIdx]) > 0) {
            numMoves++;
            argIdx++;
        }

        if (numMoves > 0) {
            const char** moves = (const char**)malloc(numMoves * sizeof(char*));
            if (!moves) {
                if (isDebug) printf("info string Failed to allocate moves array\n");
                return;
            }

            // Reset argIdx and copy moves
            argIdx = moveStart;
            for (int i = 0; i < numMoves; i++) {
                moves[i] = args[argIdx++];
            }

            applyMoves(moves, numMoves, &globalGameState);
            free(moves);
        }
    }
}

void handleGo(char** args) {
    SearchParams params = {0};  // Initialize all to 0/NULL/false
    int argIdx = 1;
    
    // Parse all parameters
    while (args[argIdx] != NULL && args[argIdx][0] != '\0') {
        if (!strcmp(args[argIdx], "searchmoves")) {
            // Count how many moves are listed
            int moveCount = 0;
            argIdx++;
            while (args[argIdx] != NULL && args[argIdx][0] != '\0' && 
                   strlen(args[argIdx]) >= 4) {  // Valid moves are at least 4 chars
                moveCount++;
                argIdx++;
            }
            // Allocate and store the moves
            if (moveCount > 0) {
                params.searchmoves = (const char**)malloc(moveCount * sizeof(char*));
                params.num_searchmoves = moveCount;
                argIdx -= moveCount;
                for (int i = 0; i < moveCount; i++) {
                    size_t len = strlen(args[argIdx]) + 1;  // +1 for null terminator
                    char* move = (char*)malloc(len);
                    strcpy(move, args[argIdx]);
                    params.searchmoves[i] = move;
                    argIdx++;
                }
            }
        }
        else if (!strcmp(args[argIdx], "ponder")) {
            params.ponder = true;
            pondering = true;  // Set global pondering state
            argIdx++;
        }
        else if (!strcmp(args[argIdx], "wtime")) {
            argIdx++;
            if (args[argIdx]) params.wtime = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "btime")) {
            argIdx++;
            if (args[argIdx]) params.btime = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "winc")) {
            argIdx++;
            if (args[argIdx]) params.winc = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "binc")) {
            argIdx++;
            if (args[argIdx]) params.binc = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "movestogo")) {
            argIdx++;
            if (args[argIdx]) params.movestogo = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "depth")) {
            argIdx++;
            if (args[argIdx]) params.depth = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "nodes")) {
            argIdx++;
            if (args[argIdx]) params.nodes = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "mate")) {
            argIdx++;
            if (args[argIdx]) params.mate = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "movetime")) {
            argIdx++;
            if (args[argIdx]) params.movetime = atoi(args[argIdx++]);
        }
        else if (!strcmp(args[argIdx], "infinite")) {
            params.infinite = true;
            argIdx++;
        }
        else {
            // Unknown parameter, skip it
            argIdx++;
        }
    }

    // Start search in a non-blocking way
    searching = 1;

    // Calculate time to use for this move
    int timeToUse = 0;
    if (params.movetime > 0) {
        timeToUse = params.movetime;
    } else if (globalGameState.toMove == 'w' && params.wtime > 0) {
        // Use ~1/20th of remaining time if no moves to go specified
        int movesToGo = params.movestogo > 0 ? params.movestogo : 20;
        timeToUse = params.wtime / movesToGo;
        if (params.winc > 0) timeToUse += params.winc * 3 / 4;
    } else if (globalGameState.toMove == 'b' && params.btime > 0) {
        int movesToGo = params.movestogo > 0 ? params.movestogo : 20;
        timeToUse = params.btime / movesToGo;
        if (params.binc > 0) timeToUse += params.binc * 3 / 4;
    }

    // TODO: Start actual search here when analysis.c is implemented
    // For now just output a dummy move after showing some search info
    if (!params.infinite && !params.ponder) {
        printf("info depth 1 score cp 100 time %d nodes 1 nps 1 pv e2e4\n", timeToUse);
        printf("bestmove e2e4\n");
        searching = 0;
    }

    // Clean up
    if (params.searchmoves) {
        for (int i = 0; i < params.num_searchmoves; i++) {
            free((void*)params.searchmoves[i]);
        }
        free(params.searchmoves);
    }
}

void handlePonderhit(void) {
    if (!pondering) {
        // If we're not pondering, ignore the command as per UCI spec
        return;
    }
    
    // Convert ponder search to a normal search
    pondering = false;
    
    // TODO: When analysis.c is implemented:
    // 1. Keep the current search tree
    // 2. Update time management
    // 3. Continue searching normally
    
    // For now, just return a move
    printf("info depth 1 score cp 100 time 1000 nodes 1 nps 1 pv e2e4\n");
    printf("bestmove e2e4\n");
    searching = 0;
}

void stopSearch(void) {
    if (searching) {
        searching = 0;
        printf("bestmove e2e4\n"); // Dummy move for now
    }
}

void argParse(const char* inputBuffer, char** args) {
    if (!inputBuffer || !args) return;

    size_t inputLen = strlen(inputBuffer);
    size_t argNum = 0;
    size_t argChar = 0;
    bool inArg = false;

    // Process each character
    for (size_t i = 0; i < inputLen && argNum < MAX_ARGS; i++) {
        char c = inputBuffer[i];

        if (c == ' ' || c == '\t') {
            if (inArg) {
                // End current argument
                if (argChar < MAX_ARG_LENGTH - 1) {
                    args[argNum][argChar] = '\0';
                    argNum++;
                    argChar = 0;
                    inArg = false;
                }
            }
        } else {
            if (!inArg) {
                // Start new argument
                inArg = true;
                argChar = 0;
            }
            
            // Add character to current argument if there's room
            if (argChar < MAX_ARG_LENGTH - 1) {
                args[argNum][argChar++] = c;
            }
        }
    }

    // Terminate last argument if we were processing one
    if (inArg && argNum < MAX_ARGS && argChar < MAX_ARG_LENGTH - 1) {
        args[argNum][argChar] = '\0';
        argNum++;
    }

    // Ensure remaining arguments are empty
    for (size_t i = argNum; i < MAX_ARGS; i++) {
        args[i][0] = '\0';
    }
}

void processInput(const char* inputBuffer, size_t inputSize) {
    if (!inputBuffer || inputSize == 0 || inputSize > INPUT_BUFFER_SIZE) {
        if (isDebug) printf("info string Invalid input buffer\n");
        return;
    }

    // Allocate argument array
    char** args = NULL;
    bool shouldExit = false;  // Flag to indicate if we should exit after cleanup
    args = (char**)malloc(MAX_ARGS * sizeof(char*));
    if (!args) {
        if (isDebug) printf("info string Memory allocation failed\n");
        return;
    }

    // Initialize all pointers to NULL for safer cleanup
    memset(args, 0, MAX_ARGS * sizeof(char*));

    // Allocate space for each argument
    for (int i = 0; i < MAX_ARGS; i++) {
        args[i] = (char*)malloc(MAX_ARG_LENGTH * sizeof(char));
        if (!args[i]) {
            // Cleanup previously allocated memory
            for (int j = 0; j < i; j++) {
                free(args[j]);
            }
            free(args);
            if (isDebug) printf("info string Memory allocation failed\n");
            return;
        }
        args[i][0] = '\0';
    }

    argParse(inputBuffer, args);

    // Validate first argument exists
    if (args[0][0] == '\0') {
        if (isDebug) printf("info string Empty command\n");
        goto cleanup;
    }

    // Find command in command list
    uint8_t inputIndex;
    bool found = false;
    for (inputIndex = 0; inputIndex < INPUT_STRINGS_LEN; inputIndex++) {
        if (!strcmp(args[0], commands[inputIndex])) {
            found = true;
            break;
        }
    }

    if (!found) {
        if (isDebug) printf("info string Unknown command: %s\n", args[0]);
        goto cleanup;
    }

    // Process command
    switch (inputIndex) {
        case UCI:
            printf("id name %s %s\n", getProgramName(), getVersionNumber());
            printf("id author %s\n", getAuthor());
            // TODO: Send options here when implemented
            printf("uciok\n");
            break;

        case DEBUG:
            if (!args[DEBUG_SWITCH_ARG_IDX] || 
                (strcmp(args[DEBUG_SWITCH_ARG_IDX], DEBUG_ON) && 
                 strcmp(args[DEBUG_SWITCH_ARG_IDX], DEBUG_OFF))) {
                if (isDebug) printf("info string Invalid debug argument. Usage: debug [on|off]\n");
            } else if (!strcmp(args[DEBUG_SWITCH_ARG_IDX], DEBUG_ON)) {
                if (isDebug) printf("info string debug turned on\n");
            } else {
                if (isDebug) printf("info string debug turned off\n");
            }
            break;

        case IS_READY:
            printf("readyok\n");
            break;

        case REGISTRATION:
            printf("registration checking\nregistration ok\n");
            break;
        
        case POSITION:
            handlePosition(args);
            break;
            
        case GO:
            handleGo(args);
            break;
            
        case STOP:
            stopSearch();
            break;
            
        case PONDERHIT:
            handlePonderhit();
            break;
            
        case QUIT:
            shouldExit = true;
            goto cleanup;
            break;
            
        default:
            if (isDebug) printf("info string Command processing error\n");
            break;
    }

cleanup:
    // Clean up allocated memory
    for (int i = 0; i < MAX_ARGS; i++) {
        free(args[i]);
    }
    free(args);
    
    if (shouldExit) {
        exit(0);  // Only exit if flag is set
    }
}

void* listenForInput(void* arg) {
    char input[INPUT_BUFFER_SIZE];
    
    // Make stdin unbuffered to handle input immediately
    setvbuf(stdin, NULL, _IONBF, 0);
    
    while (1) {
        // Clear the input buffer before reading
        memset(input, 0, INPUT_BUFFER_SIZE);
        
        // Read a line of input, handling EOF and errors
        if (fgets(input, INPUT_BUFFER_SIZE, stdin) == NULL) {
            // Check if it's EOF or an error
            if (feof(stdin)) {
                // EOF - engine should exit
                if (isDebug) printf("info string Received EOF, exiting\n");
                exit(0);
            } else {
                // Error reading input - report but continue
                if (isDebug) printf("info string Error reading input\n");
                clearerr(stdin);
                continue;
            }
        }
        
        // Remove trailing newline if present
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
            len--;
        }
        
        // Check if line was too long (no newline found)
        if (len == INPUT_BUFFER_SIZE - 1) {
            if (isDebug) printf("info string Input too long, truncated\n");
            clearInputBuffer(); // Clear remaining input
        }
        
        // Skip empty lines
        if (len == 0) {
            continue;
        }
        
        // Process the input
        processInput(input, len);
    }
    
    return NULL;
}
