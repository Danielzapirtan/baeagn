#define _GNU_SOURCE
#include <jni.h>
#include <stdlib.h>
#include <string.h>

#define _NOEDIT 0
#define _BAEAGN_ANDROID 1
#define main baeagn_cli_main
#include "../../../../baeagn.c"
#undef main

static int piece_code(char piece)
{
    switch (piece) {
        case 'P': return _WP;
        case 'N': return _WN;
        case 'B': return _WB;
        case 'R': return _WR;
        case 'Q': return _WQ;
        case 'K': return _WK;
        case 'p': return _BP;
        case 'n': return _BN;
        case 'b': return _BB;
        case 'r': return _BR;
        case 'q': return _BQ;
        case 'k': return _BK;
        default: return 0;
    }
}

static int read_fen(const char *fen, BOARD board, int *black_to_move)
{
    char copy[256];
    char *saveptr = NULL;
    char *placement;
    char *active;
    char *castling;
    char *en_passant;
    int rank = 7;
    int file = 0;
    int i;

    if (strlen(fen) >= sizeof(copy))
        return 0;
    strcpy(copy, fen);
    placement = strtok_r(copy, " ", &saveptr);
    active = strtok_r(NULL, " ", &saveptr);
    castling = strtok_r(NULL, " ", &saveptr);
    en_passant = strtok_r(NULL, " ", &saveptr);
    if (!placement || !active || !castling || !en_passant ||
        (strcmp(active, "w") && strcmp(active, "b")))
        return 0;

    memset(board, 0, sizeof(BOARD));
    for (i = 0; placement[i] && rank >= 0; i++) {
        char c = placement[i];
        if (c == '/') {
            if (file != 8)
                return 0;
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += c - '0';
            if (file > 8)
                return 0;
        } else {
            int code = piece_code(c);
            if (!code || file >= 8)
                return 0;
            board[rank][file++] = code;
        }
    }
    if (rank != 0 || file != 8)
        return 0;
    board[8][0] = strchr(castling, 'Q') != NULL;
    board[8][1] = strchr(castling, 'K') != NULL;
    board[8][2] = strchr(castling, 'q') != NULL;
    board[8][3] = strchr(castling, 'k') != NULL;
    board[8][4] = -1;
    if (strcmp(en_passant, "-")) {
        if (strlen(en_passant) != 2 || en_passant[0] < 'a' || en_passant[0] > 'h' ||
            en_passant[1] != (active[0] == 'w' ? '6' : '3'))
            return 0;
        board[8][4] = en_passant[0] - 'a';
    }
    *black_to_move = active[0] == 'b';
    stm = *black_to_move;
    if (*black_to_move)
        transpose(board);
    return 1;
}

static void square_name(int x, int y, int black_to_move, char out[3])
{
    out[0] = (char)('a' + x);
    out[1] = (char)('1' + (black_to_move ? 7 - y : y));
    out[2] = '\0';
}

JNIEXPORT jstring JNICALL
Java_org_baeagn_app_Engine_nativeBestMove(JNIEnv *env, jobject thiz, jstring jfen, jint requested_depth)
{
    (void)thiz;
    const char *fen = (*env)->GetStringUTFChars(env, jfen, NULL);
    if (!fen)
        return NULL;

    BOARD start;
    int black_to_move = 0;
    if (!read_fen(fen, start, &black_to_move)) {
        (*env)->ReleaseStringUTFChars(env, jfen, fen);
        return (*env)->NewStringUTF(env, "");
    }
    (*env)->ReleaseStringUTFChars(env, jfen, fen);

    static TREE search_tree[_MAXLEVEL];
    static TREE ordering_tree[_MAXLEVEL];
    memset(search_tree, 0, sizeof(search_tree));
    memset(ordering_tree, 0, sizeof(ordering_tree));
    treea = search_tree;
    treeb = ordering_tree;
    load_values();
    init(&elapsed);
    nodes = 0;
    pvsready = 0;
    newpv = 0;
    glevel = 0;
    int depth = requested_depth < 1 ? 1 : requested_depth > 6 ? 6 : requested_depth;
    gdepth = depth + _OVERDEPTH;
    search_tree[0].level = 0;
    search_tree[0].depth = gdepth;
    search_tree[0].alpha = _ALPHA;
    search_tree[0].beta = _BETA;
    copy_board(start, search_tree[0].curr_board);
    (void)search(treea, 0, 1);

    char result[6] = "";
    MOVE move;
    copy_move(search_tree[0].best_line[0], move);
    if (search_tree[0].bl_len > 0) {
        char from[3], to[3];
        square_name(move[1], move[0], black_to_move, from);
        square_name(move[3], move[2], black_to_move, to);
        snprintf(result, sizeof(result), "%s%s", from, to);
        if (move[4]) {
            size_t length = strlen(result);
            const char promotion[] = { ' ', ' ', 'n', 'b', 'r', 'q' };
            if (move[4] >= 2 && move[4] <= 5) {
                result[length] = promotion[move[4]];
                result[length + 1] = '\0';
            }
        }
    }
    return (*env)->NewStringUTF(env, result);
}
