#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uci.h"
#include "version.h"
#include "setup.h"
#include "pieces.h"
#include "gameState.h"
#include "moves.h"

// CONSTANTS
// "debug" command constants
#define DEBUG_SWITCH_ARG_IDX 1
#define DEBUG_ON "on"
#define DEBUG_OFF "off"

// Longest reasonable command would be "position fen" followed by longest possible FEN
// plus a long sequence of moves
#define INPUT_BUFFER_SIZE 8192

volatile int searching = 0;
volatile bool pondering = false;
volatile bool isDebug = false;

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

void parseMoves(char args[MAX_ARGS][MAX_ARG_LENGTH], int numMoves, int moveStart) {
    const char** moves = (const char**)malloc(numMoves * sizeof(char*));
    if (!moves) {
        if (isDebug) printf("info string Failed to allocate moves array\n");
        return;
    }

    for (int i = 0; i < numMoves; i++) {
        moves[i] = args[moveStart++];
    }
    
    applyMoves(moves, numMoves, &globalGameState);
    free(moves);
}

void handlePosition(char args[MAX_ARGS][MAX_ARG_LENGTH]) {
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
        while (args[argIdx] && strcmp(args[argIdx], "moves")) {
            strcat(fen, args[argIdx]);
            strcat(fen, " ");
            argIdx++;
        }
    } else {
        if (isDebug) printf("info string Expected 'startpos' or 'fen', got: %s\n", args[argIdx]);
        return;
    }
    
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

        if (numMoves > 0)
            parseMoves(args, numMoves, moveStart);
    }
}

void handleGo(char args[MAX_ARGS][MAX_ARG_LENGTH]) {
    SearchParams params = {0};
    int argIdx = 1;
    
    // Parse all parameters
    while (args[argIdx] != NULL && args[argIdx][0] != '\0') {
        if (!strcmp(args[argIdx], "searchmoves")) {
            argIdx++;
            if (!args[argIdx]) {
                if (isDebug) printf("info string 'searchmoves' specified but no moves given\n");
                return;
            }
            
            int numMoves = 0;
            int moveStart = argIdx;
            while (args[argIdx] != NULL && strlen(args[argIdx]) > 0) {
                numMoves++;
                argIdx++;
            }
            
            if (numMoves > 0)
                parseMoves(args, numMoves, moveStart);
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

void argParse(const char* inputBuffer, char args[MAX_ARGS][MAX_ARG_LENGTH]) {
    if (!inputBuffer || !args) return;

    size_t inputLen = strlen(inputBuffer);
    size_t argNum = 0;
    size_t argChar = 0;
    bool inArg = false;

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

    for (size_t i = argNum; i < MAX_ARGS; i++) {
        args[i][0] = '\0';
    }
}

void processInput(const char* inputBuffer, size_t inputSize) {
    if (!inputBuffer || inputSize == 0 || inputSize > INPUT_BUFFER_SIZE) {
        if (isDebug) printf("info string Invalid input buffer\n");
        return;
    }

    char args[MAX_ARGS][MAX_ARG_LENGTH];

    for (int i = 0; i < MAX_ARGS; i++) {
        args[i][0] = '\0';
    }

    argParse(inputBuffer, args);

    if (args[0][0] == '\0') {
        if (isDebug) printf("info string Empty command\n");
        return;
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
        return;
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
                isDebug = true;
                if (isDebug) printf("info string debug turned on\n");
            } else {
                isDebug = false;
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
            if (isDebug) printf("info string Quitting\n");
            exit(0);
            break;
            
        default:
            if (isDebug) printf("info string Command processing error\n");
            break;
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
            if (feof(stdin)) {
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
            clearInputBuffer();
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
