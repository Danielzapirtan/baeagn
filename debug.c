#include <ctype.h>

#define MAX_MOVES 500

// Function to initialize the starting chess position
void init_board(BOARD board) {
    int starting_pos[9][8] = {
        {-4, -2, -3, -5, -6, -3, -2, -4}, // black pieces
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},         // white pawns
        {4, 2, 3, 5, 6, 3, 2, 4},         // white pieces
        {0, 0, 0, 0, 0, 0, 0, 0}          // extra row (unused in standard chess)
    };
    memcpy(board, starting_pos, sizeof(starting_pos));
}

// Function to convert SAN to move coordinates
int san_to_move(BOARD board, const char *san, MOVE move, int white_turn) {
    MOVELIST movelist;
    int count = gen(board, movelist, 1);
    
    memset(move, 0, sizeof(MOVE));
    
    // Handle castling
    if (strcmp(san, "O-O") == 0 || strcmp(san, "0-0") == 0) {
        int row = white_turn ? 7 : 0;
        move[0] = row;
        move[1] = 4;
        move[2] = row;
        move[3] = 6;
        return 1;
    }
    if (strcmp(san, "O-O-O") == 0 || strcmp(san, "0-0-0") == 0) {
        int row = white_turn ? 7 : 0;
        move[0] = row;
        move[1] = 4;
        move[2] = row;
        move[3] = 2;
        return 1;
    }
    
    // Parse promotion
    char promote = 0;
    int len = strlen(san);
    if (len >= 2 && san[len-2] == '=') {
        promote = san[len-1];
        len -= 2;
    }
    
    // Parse destination square
    if (len < 2) return 0;
    move[3] = tolower(san[len-2]) - 'a';
    move[2] = '8' - san[len-1];
    if (move[3] < 0 || move[3] > 7 || move[2] < 0 || move[2] > 7) return 0;
    len -= 2;
    
    // Parse piece type (default to pawn)
    char piece = 'P';
    if (len > 0 && isupper(san[0])) {
        piece = san[0];
        len--;
    }
    
    // Find matching move in movelist
    for (int i = 0; i < count; i++) {
        MOVE m;
        copy_move(movelist[i], m);
        int piece_val = board[m[0]][m[1]];
        if ((piece == 'P' && abs(piece_val) != 1) ||
            (piece == 'N' && abs(piece_val) != 2) ||
            (piece == 'B' && abs(piece_val) != 3) ||
            (piece == 'R' && abs(piece_val) != 4) ||
            (piece == 'Q' && abs(piece_val) != 5) ||
            (piece == 'K' && abs(piece_val) != 6)) continue;
            
        if (m[2] != move[2] || m[3] != move[3]) continue;
        
        // Check promotion
        if (promote && ((promote == 'Q' && abs(piece_val) != 1) || 
                        (promote == 'R' && abs(piece_val) != 4) ||
                        (promote == 'B' && abs(piece_val) != 3) ||
                        (promote == 'N' && abs(piece_val) != 2))) continue;
        
        // Check disambiguation
        if (len > 0) {
            if (isdigit(san[0])) { // row disambiguation
                int row = '8' - san[0];
                if (m[0] != row) continue;
            } else { // column disambiguation
                int col = tolower(san[0]) - 'a';
                if (m[1] != col) continue;
            }
        }
        
        copy_move(m, move);
        move[4] = promote;
        return 1;
    }
    
    return 0;
}

void parse_pgn(BOARD board) {
    FILE *file = fopen("start.pgn", "r");
    if (!file) {
        printf("Error opening PGN file\n");
        return;
    }
    
    copy_board(*get_init(), board);
    
    char line[256];
    int move_number = 0;
    int white_turn = 1;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip header lines
        if (line[0] == '[') continue;
        
        // Tokenize move text
        char *token = strtok(line, " \t\n");
        while (token) {
            // Skip move numbers
            if (strstr(token, ".")) {
                token = strtok(NULL, " \t\n");
                continue;
            }
            
            // Skip result (1-0, 0-1, 1/2-1/2)
            if (strcmp(token, "1-0") == 0 || strcmp(token, "0-1") == 0 || 
                strcmp(token, "1/2-1/2") == 0 || strcmp(token, "*") == 0) {
                break;
            }
            
            // Process move
            MOVE move;
            if (san_to_move(board, token, move, white_turn)) {
                BOARD aux;
                makemove(board, move, aux);
                copy_board(aux, board);
                show_board(board, stdout);
                white_turn = !white_turn;
            } else {
                printf("Error parsing move: %s\n", token);
            }
            
            token = strtok(NULL, " \t\n");
        }
    }
    
    fclose(file);
}
