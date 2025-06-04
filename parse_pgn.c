#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define MAX_MOVES 500

// Function to convert SAN to move coordinates
int san_to_move(BOARD board, const char *san, MOVE move, int white_turn) {
    MOVELIST movelist;
    // Assuming gen() generates all possible moves
    show_board(board, stdout);
    int count = gendeep(board, movelist, 0);
    printf("%d moves\n", count);
    fflush(stdout);
    
    // Handle castling
    if (strcmp(san, "O-O") == 0 || strcmp(san, "0-0") == 0) {
        int row = 0;
        move[0] = row;
        move[1] = 4;
        move[2] = row;
        move[3] = 6;
        return 1;
    }
    if (strcmp(san, "O-O-O") == 0 || strcmp(san, "0-0-0") == 0) {
        int row = 0;
        move[0] = row;
        move[1] = 4;
        move[2] = row;
        move[3] = 2;
        return 1;
    }
    
    // Parse promotion
    char promote = 0;
    int len = strlen(san);
    if (len >= 4 && san[len-2] == '=') {
        promote = san[len-1];
        len -= 2;
    }
    
    // Parse destination square
    if (len < 2) return 0;
    move[3] = tolower(san[len-2]) - 'a';
    move[2] = san[len-1] - '1';
    if (white_turn)
	    move[2] = 7 - move[2];
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
//	printf("move: %d%d%d%d\n", m[1], m[0], m[3], m[2]);
//	fflush(stdout);
        int piece_val = board[m[0]][m[1]];
        if ((piece == 'P' && abs(piece_val) != 1) ||
            (piece == 'N' && abs(piece_val) != 2) ||
            (piece == 'B' && abs(piece_val) != 3) ||
            (piece == 'R' && abs(piece_val) != 4) ||
            (piece == 'Q' && abs(piece_val) != 5) ||
            (piece == 'K' && abs(piece_val) != 6)) continue;
            
        if (m[2] != move[2] || m[3] != move[3]) continue;
        
	// Check promotion
        if (promote && ((promote == 'Q' && abs(piece_val) != 5) || 
                        (promote == 'R' && abs(piece_val) != 4) ||
                        (promote == 'B' && abs(piece_val) != 3) ||
                        (promote == 'N' && abs(piece_val) != 2))) continue;
        
        if (len > 0) {
	    char s0 = san[isupper(san[0])];
            if (isdigit(san[0])) { // row disambiguation
                int row = san[0] - 1;
		if (white_turn)
			row = 7 - row;
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
    
    // Initialize board
    copy_board(*get_init(), board);
    show_board(board, stdout);
    
    char lline[256];
    char *line = lline;
    int white_turn = 0;
    
    while (fgets(line, 256, file)) {
        // Skip header lines
	printf("%s\n", line);
	fflush(stdout);
        if (line[0] == '[') continue;
        
        // Tokenize move text
        while (1) {
	    char *cline = line;
	    char *token;
	    while (1) {
		    if (isblank(*cline)) {
			    *cline = '\0';
			    token = line;
			    line = cline + 1;
			    break;
		    }
		    cline++;
            }
	    if (!cline)
		    break;
	    printf("Token: %s\n", token);
	    fflush(stdout);
            // Skip result (1-0, 0-1, 1/2-1/2)
            if (strcmp(token, "1-0") == 0 || strcmp(token, "0-1") == 0 || 
                strcmp(token, "1/2-1/2") == 0 || strcmp(token, "*") == 0) {
                break;
            }
            
	    if (token[strlen(token) - 1] == '.')
		    continue;
            // Process move
            MOVE move;
            if (san_to_move(board, (const char *)  token, move, white_turn)) {
		printf("%c%c%c%c\n", move[1] + 97, move[0] + 49, move[3] + 97, move[2] + 49);
		fflush(stdout);
                BOARD new_board;
                // Assuming makemove makes a move and returns new board
                makemove(board, move, new_board);
                copy_board(new_board, board);
                white_turn = !white_turn;
            } else {
                printf("Error parsing move: %s\n", token);
            }
            
        }
    }
    
    fclose(file);
}
