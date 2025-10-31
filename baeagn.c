#define _GNU_SOURCE

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifndef _NOEDIT
#define _NOEDIT (1)
#endif
#define _ALLOW_CASTLE (1)
#define _DEBUG (0)
#define _GAME_LOST (800)
#ifndef _MAXINDEX
#define _MAXINDEX (200)
#endif
#define _MAXLEVEL (32)
#define _FRAMESPERSEC (32)
#define _NPS (3 << 20)
#define _SKIPFRAMES (_NPS / _FRAMESPERSEC)
#define _BRDFILE "start.brd"
#define _FENFILE "start.fen"

#define _ALPHA (-20000) // Adjusted as needed
#define _BETA (20000)
#define _OVERDEPTH (1)
#define _S_DEPTH (4)
#define _SORT
#define _PVSEARCH
#define _SVP
#undef _CAND7
#undef _CAND250
#define _CANDCUT (200)
#undef _Q0BLK // For opening phase, block Queen's moves at node root

#ifndef _PIECE_CODES
#define _PIECE_CODES (1)
#define _WP (1)
#define _WN (2)
#define _WB (3)
#define _WR (4)
#define _WQ (5)
#define _WK (6)
#define _BP (-1)
#define _BN (-2)
#define _BB (-3)
#define _BR (-4)
#define _BQ (-5)
#define _BK (-6)
#define _WM (7)
#define _BM (-7)
#define _UO (0)
#define _BS (-8)
#endif

typedef signed int s3;
typedef signed int s4;
typedef signed int s5;
typedef signed long long s6;
typedef unsigned int u3;
typedef unsigned int u4;
typedef unsigned int u5;
typedef unsigned long long u6;

typedef u6 NODES;
typedef double TIME;
typedef s3 MOVE[6];
typedef MOVE MOVELIST[_MAXINDEX];
typedef s3 BOARD[9][8];
typedef s4 VALUE;
typedef u4 LEVEL;
typedef u4 MOVEINDEX;

typedef struct {
    int seconds;
    int useconds;
    clock_t clock0;
    clock_t clock1;
    int diffclock;
} ELAPSED;

typedef struct {
    BOARD curr_board;
    BOARD next_board;
    LEVEL bl_len;
    LEVEL depth;
    LEVEL level;
    MOVE best_line[_MAXLEVEL];
    MOVE curr_move;
    MOVEINDEX curr_index;
    MOVEINDEX max_index;
    MOVELIST legal_moves;
    VALUE alpha;
    VALUE best;
    VALUE beta;
    VALUE value;
    VALUE valuelist[_MAXINDEX];
} TREE;

extern ELAPSED elapsed;
extern LEVEL gdepth;
extern LEVEL glevel;
extern TREE *treea;
extern TREE *treeb;
extern MOVE best_move;
extern NODES nodes;
extern int newpv;
extern int pvsready;
extern s4 gmode;
extern s4 stm;

extern void init(ELAPSED *elapsed);
extern void update(ELAPSED *elapsed);
extern double dclock(ELAPSED *elapsed);
extern void addm(s5 y, s5 x, s5 y1, s5 x1, MOVEINDEX *curr_index, MOVELIST movelist);
extern void addprom(s5 y, s5 x, s5 y1, s5 x1, s5 to, MOVEINDEX *curr_index, MOVELIST movelist);
extern void analysis(void);
extern VALUE search(TREE *tree_, LEVEL level, LEVEL depth);
extern int board_cmp(BOARD src, BOARD dest);
extern void copy_board(BOARD src, BOARD dest);
extern void copy_move(MOVE src, MOVE dest);
extern void castle(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
extern void warn(const char *msg);
extern VALUE eval(BOARD board, LEVEL level);
extern void genP(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
extern void genN(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
extern void genB(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
extern void genR(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
extern void genQ(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
extern void genK(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist, LEVEL depth);
extern MOVEINDEX gendeep(BOARD board, MOVELIST movelist, LEVEL depth);
extern MOVEINDEX gen(BOARD board, MOVELIST movelist, LEVEL level);
extern BOARD *get_init(void);
extern void load(BOARD start);
extern s4 in_check(BOARD board);
extern s4 is_pv(LEVEL level);
extern void makemove(BOARD src, MOVE move, BOARD dest);
extern s4 move_cmp(MOVE src, MOVE dest);
extern void nonslider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist);
extern void show_move(MOVE move, BOARD board, u5 stm, char *buf);
extern void slider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist);
extern void show_board(BOARD board, FILE *f);
extern void transpose(BOARD board);
extern void setup_board(BOARD board);
extern void parse_fen(BOARD board);
extern void save(BOARD board);

const VALUE _ALPHA_DFL    = (-20000);
const VALUE _BETA_DFL     = (+20000);
const VALUE _MAXVALUE     = (20000);
const VALUE _PAWNUNIT     = (100);
const VALUE _THRESHOLD    = (15000);
VALUE _VALUES[6];

ELAPSED elapsed;
LEVEL gdepth;
LEVEL glevel;
MOVE best_move;
NODES nodes;
TREE *treea;
TREE *treeb;
int newpv;
int pvsready;
s4 gmode;
s4 stm;

void analysis(void)
{
    BOARD aux;
    BOARD aux2;
    BOARD start;
    char buf[80];
    LEVEL depth;
    LEVEL i;
    TREE *tree;
    s4 ix = 0;
#if _NOEDIT == 3
    parse_pgn();
    exit(0);
#elif _NOEDIT == 2
    parse_fen(start);
    save(start);
#elif _NOEDIT == 1
    parse_pgn();
    load(start);
#else
    load(start);
//    setup_board(start);
//    copy_board(*get_init(), start);
//    save(start);
#endif
    show_board(start, stdout);
    treea = (TREE *) malloc (_MAXLEVEL * sizeof(TREE));
    if (!treea) {
        warn("Out of memory!");
    }
    treeb = (TREE *) malloc(_MAXLEVEL * sizeof(TREE));
    if (!treeb)
        warn("Out of memory");
    init(&elapsed);
    nodes = 0LL;
    pvsready = 0;
    for (depth = _S_DEPTH + 1; depth < _MAXLEVEL; depth++) {
        tree = &treea[0];
        copy_board(start, tree->curr_board);
        tree->level = 0;
        tree->depth = depth + _OVERDEPTH;
        gdepth = tree->depth;
        tree->alpha = _ALPHA;
        tree->beta = _BETA;
        newpv = 0;
        tree->best = search(treea, 0, 1);
        pvsready = 1;
        update(&elapsed);
        double delapsed = dclock(&elapsed);
        copy_board(start, aux);
        fprintf(stdout, "Depth: %u\n", depth);
        fprintf(stdout, "Evaluation: %.2lf\n", \
            ((double) (tree->best) / (double) _PAWNUNIT));
        fprintf(stdout, "Branching factor: %.2lf\n", pow((double) nodes, (double) 1 / (depth)));
        fprintf(stdout, "Best variation: ");
        for (i = 0; i < tree->bl_len; i++) {
            show_move(tree->best_line[i], aux, (i + stm) % 2, buf);
            makemove(aux, tree->best_line[i], aux2);
            copy_board(aux2, aux);
            fprintf(stdout, "%s ", buf);
        }
        fprintf(stdout, "\n");
        if (tree->bl_len & 1)
            transpose(aux);
        fprintf(stdout, "Elapsed: %.2lf\n", delapsed);
        fprintf(stdout, "NPS: %u\n", (unsigned int) ((double) nodes / delapsed));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    free(treea);
    free(treeb);
}

VALUE search(TREE *tree_, LEVEL level, LEVEL depth)
// level means distance from root
// depth means 1 if treea, 0 if treeb
{
    BOARD aux;
    BOARD aux2;
    char buf[80];
    LEVEL bl_lev;
    LEVEL i;
    TREE *tree;
    TREE *ntree;
    VALUE value;
    tree = &tree_[level];
    value = eval(tree->curr_board, level);
    if (newpv)
	tree->bl_len = 0;
    if (value < -_THRESHOLD) {
        return (value);
    }
    if (tree->depth == 0) {
        return (value);
    }
    if (tree->depth <= _OVERDEPTH)
    if (value > -(_PAWNUNIT >> 1)) {
        return (value);
    }
    if (depth)
        glevel = level;
    tree->max_index = gen(tree->curr_board, tree->legal_moves, depth);
    if (tree->max_index == 0) {
        return (-_MAXVALUE + level);
    }
    if (newpv)
	tree->bl_len = 1;
    tree->best = -_MAXVALUE;
    for (tree->curr_index = 0; tree->curr_index < tree->max_index; (tree->curr_index)++) {
        ntree = &tree_[level + 1];
        copy_move(tree->legal_moves[tree->curr_index], tree->curr_move);
        makemove(tree->curr_board, tree->curr_move, tree->next_board);
        copy_board(tree->next_board, ntree->curr_board);
#ifdef _SVP
        if (depth)
        if (level < 1)
        if (tree->curr_index) {
            ntree->level = tree->level + 1;
            ntree->depth = tree->depth - 1;
            ntree->alpha = -(tree->alpha) - 1;
            ntree->beta = -(tree->alpha);
            tree->value = -search(tree_, level + 1, depth);
            if (tree->value <= tree->alpha)
                continue;
        }
#endif
        ntree->level = tree->level + 1;
        ntree->depth = tree->depth - 1;
        ntree->alpha = -(tree->beta);
        ntree->beta = -(tree->alpha);
        tree->value = -search(tree_, level + 1, depth);
        if (!newpv)
            ntree->bl_len = 0;
        newpv = 1;
        if (tree->value > tree->best) {
            tree->best = tree->value;
            tree->bl_len = ntree->bl_len + 1;
            copy_move(tree->curr_move, tree->best_line[0]);
            if (ntree->bl_len > 0)
            for (bl_lev = 0; bl_lev < ntree->bl_len; bl_lev++)
                copy_move(ntree->best_line[bl_lev], \
                    tree->best_line[bl_lev + 1]);
            if (level == 0 && depth == 1 && gmode == 4) {
                update(&elapsed);
                double delapsed = dclock(&elapsed);
                copy_board(treea->curr_board, aux);
                fprintf(stdout, "Depth: %u*\n", treea->depth - _OVERDEPTH);
                fprintf(stdout, "Evaluation: %.2lf\n", \
                    ((double) treea->best / (double) _PAWNUNIT));
                fprintf(stdout, "Branching factor: %.2lf\n", pow((double) nodes, (double) 1 / (treea->depth - _OVERDEPTH)));
                fprintf(stdout, "Best variation: ");
                for (i = 0; i < treea->bl_len; i++) {
                    show_move(treea->best_line[i], aux, (i + stm) % 2, buf);
                    makemove(aux, treea->best_line[i], aux2);
                    copy_board(aux2, aux);
                    fprintf(stdout, "%s ", buf);
                }
                fprintf(stdout, "\n");
                if (treea->bl_len & 1)
                    transpose(aux);
                fprintf(stdout, "Elapsed: %.2lf\n", delapsed);
		fprintf(stdout, "NPS: %u\n", (unsigned int) ((double) nodes / delapsed));
                fprintf(stdout, "\n");
                fflush(stdout);
            }
            if (tree->best > tree->alpha)
                tree->alpha = tree->best;
            if (tree->alpha >= tree->beta)
                return (tree->beta);
        }
    }
    return (tree->best);
}

int pcsq[6][8][8] = {
    // Pawn piece-square table
    {
        // Rank 8 (black perspective)
        {   0,   0,   0,   0,   0,   0,   0,   0 },
        // Rank 7
        {  10,  10,  10,  10,  10,  10,  10,  10 },
        // Rank 6
        {  25,  25,  25,  25,  25,  25,  25,  25 },
        // Rank 5
        {  35,  35,  35,  40,  40,  35,  35,  35 },
        // Rank 4
        {  45,  45,  45,  50,  50,  45,  45,  45 },
        // Rank 3
        {  60,  60,  60,  65,  65,  60,  60,  60 },
        // Rank 2
        {  80,  80,  80,  85,  85,  80,  80,  80 },
        // Rank 1 (white perspective)
        {   0,   0,   0,   0,   0,   0,   0,   0 }
    },
    
    // Knight piece-square table
    {
        { -50, -40, -30, -30, -30, -30, -40, -50 },
        { -40, -20,   0,   5,   5,   0, -20, -40 },
        { -30,   0,  10,  15,  15,  10,   0, -30 },
        { -30,   5,  15,  20,  20,  15,   5, -30 },
        { -30,   0,  15,  20,  20,  15,   0, -30 },
        { -30,   5,  10,  15,  15,  10,   5, -30 },
        { -40, -20,   0,   0,   0,   0, -20, -40 },
        { -50, -40, -30, -30, -30, -30, -40, -50 }
    },
    
    // Bishop piece-square table
    {
        { -20, -10, -10, -10, -10, -10, -10, -20 },
        { -10,   0,   0,   0,   0,   0,   0, -10 },
        { -10,   0,  10,  10,  10,  10,   0, -10 },
        { -10,   5,   5,  10,  10,   5,   5, -10 },
        { -10,   0,  10,  10,  10,  10,   0, -10 },
        { -10,  10,  10,  10,  10,  10,  10, -10 },
        { -10,   5,   0,   0,   0,   0,   5, -10 },
        { -20, -10, -10, -10, -10, -10, -10, -20 }
    },
    
    // Rook piece-square table
    {
        {   0,   0,   0,   5,   5,   0,   0,   0 },
        {  -5,   0,   0,   0,   0,   0,   0,  -5 },
        {  -5,   0,   0,   0,   0,   0,   0,  -5 },
        {  -5,   0,   0,   0,   0,   0,   0,  -5 },
        {  -5,   0,   0,   0,   0,   0,   0,  -5 },
        {  -5,   0,   0,   0,   0,   0,   0,  -5 },
        {   5,  10,  10,  10,  10,  10,  10,   5 },
        {   0,   0,   0,   0,   0,   0,   0,   0 }
    },
    
    // Queen piece-square table
    {
        { -20, -10, -10,  -5,  -5, -10, -10, -20 },
        { -10,   0,   0,   0,   0,   0,   0, -10 },
        { -10,   0,   5,   5,   5,   5,   0, -10 },
        {  -5,   0,   5,   5,   5,   5,   0,  -5 },
        {   0,   0,   5,   5,   5,   5,   0,  -5 },
        { -10,   5,   5,   5,   5,   5,   0, -10 },
        { -10,   0,   5,   0,   0,   0,   0, -10 },
        { -20, -10, -10,  -5,  -5, -10, -10, -20 }
    },
    
    // King piece-square table (middlegame)
    {
        { -30, -40, -40, -50, -50, -40, -40, -30 },
        { -30, -40, -40, -50, -50, -40, -40, -30 },
        { -30, -40, -40, -50, -50, -40, -40, -30 },
        { -30, -40, -40, -50, -50, -40, -40, -30 },
        { -20, -30, -30, -40, -40, -30, -30, -20 },
        { -10, -20, -20, -20, -20, -20, -20, -10 },
        {  20,  20,   0,   0,   0,   0,  20,  20 },
        {  20,  30,  10,   0,   0,  10,  30,  20 }
    }
};

#define abs(x) ((x > 0) ? (x) : ((-x)))
#define min(x, y) (((x) < (y)) ? (x) : (y))
VALUE eval(BOARD board, LEVEL level)
{
    BOARD aux;
    int ivalue = 0;
    int kings = 0;
    u5 x;
    u5 y;
    VALUE pvalue = 0;
    VALUE value;
    nodes++;
    if ((nodes % _SKIPFRAMES) == 0) {
        update(&elapsed);
    }
    for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++) {
        if (board[y][x] > 0)
            ivalue += pcsq[board[y][x] - 1][y][x];
        if (board[y][x] < 0)
            ivalue -= pcsq[-board[y][x] - 1][7 - y][x];
        switch (board[y][x]) {
        case _WP:
            switch (y) {
            case 1:
            case 2:
            case 3:
                ivalue += 100;
                break;
            case 4:
                ivalue += 120;
                break;
            case 5:
                ivalue += 200;
                break;
            case 6:
                ivalue += 400;
            default:;
            }
            break;
        case _WN:
        case _WB:
        case _WR:
        case _WQ:
            ivalue += _VALUES[(u5) board[y][x]];
            break;
        case _BP:
            switch (y) {
            case 6:
            case 5:
            case 4:
                ivalue -= 100;
                break;
            case 3:
                ivalue -= 120;
                break;
            case 2:
                ivalue -= 200;
                break;
            case 1:
                ivalue -= 400;
                break;
            default:;
            }
            break;
        case _BN:
        case _BB:
        case _BR:
        case _BQ:
            ivalue -= _VALUES[(u5) (-board[y][x])];
            break;
        case _WK: kings++; break;
        case _BK: kings--; break;
        default:;
        }
    }
    for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++) {
        u5 x1 = x;
        u5 y1 = y;
        if (x1 > 3) x1 = 7 - x1;
        if (y1 > 3) y1 = 7 - y1;
        if (board[y][x] < 0)
            pvalue -= (1 + min(x1, y1));
        else if (board[y][x] > 0)
            pvalue += (1 + min(x1, y1));
    }
    if (kings) {
    if (kings > 0)
        return ( _MAXVALUE - level);
    else
        return (-_MAXVALUE + level);
    }
    value = ivalue + pvalue;
#if 1
    if (treea[level].depth == 1) {
        copy_board(board, aux);
        transpose(aux);
        if (in_check(aux))
            return (_MAXVALUE - (level + 1));
    }
    if (treea[level].depth / 2 == 1) {
    if (in_check(board))
        return (-2000 + value + level);
    if (value > treea[level].alpha) {
        value = value * 10;
        if (value > 1500)
            value = 2000 - level;
        return (value);
        }
    }
#endif
    value += ((rand() % 7) - 3);
    if (level > 1)
        return (value + (treea[level - 2].max_index - treea[level - 1].max_index));
    return (value);
}

MOVEINDEX gen(BOARD board, MOVELIST movelist, LEVEL depth)
// depth means 1 if sortable, 0 otherwise
// FIXME
{
    MOVEINDEX max_index = gendeep(board, movelist, 1);
#ifdef _PVSEARCH
    if (pvsready)
    if (depth)
    if (!newpv)
    if (glevel < treea->bl_len) {
        for (LEVEL level = 0; level < glevel; level++)
        if (treea[level].curr_index) {
            printf("Skip level %d\n", level);
            fflush(stdout);
            goto skippvs;
        }
        MOVE move;
        MOVEINDEX curr_index;
        copy_move(treea->best_line[glevel], move);
        for (curr_index = 0; curr_index < max_index; curr_index++)
        if (!move_cmp(move, movelist[curr_index]))
            break;
        copy_move(movelist[0], movelist[curr_index]);
        copy_move(move, movelist[0]);
        return max_index;
    }
skippvs:
#endif
    if (!depth)
        return max_index;
#ifdef _SORT
    if (glevel < gdepth - _S_DEPTH - 1) {
        MOVEINDEX curr_index;
        MOVEINDEX ncurr_index;
        VALUE valuelist[_MAXINDEX];
        for (curr_index = 0; curr_index < max_index; curr_index++) {
            BOARD aux;
            MOVE move;
            copy_move(movelist[curr_index], move);
            makemove(board, move, aux);
            copy_board(aux, treeb[0].curr_board);
            treeb[0].level = 0;
            LEVEL _s_depth = _S_DEPTH;
            treeb[0].depth = _s_depth;
            treeb[0].alpha = _ALPHA_DFL;
            treeb[0].beta = _BETA_DFL;
            valuelist[curr_index] = -search(treeb, 0, 0);
        }
        for (curr_index = 0; curr_index < max_index; curr_index++)
        for (ncurr_index = curr_index + 1; ncurr_index < max_index; ncurr_index++) {
            if (valuelist[ncurr_index] > valuelist[curr_index]) {
            MOVE move;
            VALUE value;
            copy_move(movelist[ncurr_index], move);
            copy_move(movelist[curr_index], movelist[ncurr_index]);
            copy_move(move, movelist[curr_index]);
            value = valuelist[ncurr_index];
            valuelist[ncurr_index] = valuelist[curr_index];
            valuelist[curr_index] = value;
        }
    }
#ifdef _CAND7
    LEVEL newmax_index = max_index;
    if (glevel)
        newmax_index = 4;
    if (max_index > newmax_index)
        max_index = newmax_index;
#endif
#ifdef _CAND250
    if (glevel)
    for (curr_index = 0; curr_index < max_index; curr_index++)
    if (valuelist[curr_index] < valuelist[0] - _CANDCUT) {
        max_index = curr_index;
        break;
    }
#endif
    }
#endif
    return max_index;
}

void addm(s5 y, s5 x, s5 y1, s5 x1, MOVEINDEX *curr_index, MOVELIST movelist)
{
    movelist[*curr_index][0] = y;
    movelist[*curr_index][1] = x;
    movelist[*curr_index][2] = y1;
    movelist[*curr_index][3] = x1;
    (*curr_index)++;
    if (*curr_index >= _MAXINDEX)
        warn("Index too big");
}

void addprom(s5 y, s5 x, s5 y1, s5 x1, s5 to, MOVEINDEX *curr_index, MOVELIST movelist)
{
    movelist[*curr_index][0] = y;
    movelist[*curr_index][1] = x;
    movelist[*curr_index][2] = y1;
    movelist[*curr_index][3] = x1;
    movelist[*curr_index][4] = to;
    (*curr_index)++;
    if (*curr_index >= _MAXINDEX)
        warn("Index too big");
}

/*void castle(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    BOARD aux;
    if (y != 0) return;
    if (x != 4) return;
    if (board[y][x] != _WK)
        return;
    if (board[0][0] == _WR)
    if (board[0][1] == 0)
    if (board[0][2] == 0)
    if (board[0][3] == 0)
    if (board[8][0] == 1) {
        copy_board(board, aux);
        aux[0][2] = _WK;
        aux[0][3] = _WK;
        if (! in_check(aux))
            addm(0, 4, 0, 2, curr_index, movelist);
    }
    if (board[0][7] == _WR)
    if (board[0][6] == 0)
    if (board[0][5] == 0)
    if (board[8][1] == 1) {
        copy_board(board, aux);
        aux[0][5] = _WK;
        aux[0][6] = _WK;
        if (! in_check(aux))
            addm(0, 4, 0, 6, curr_index, movelist);
    }
}*/

void castle(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    BOARD aux;
    if (y != 0) return;
    if (x != 4) return;
    if (board[y][x] != _WK)
        return;
    
#if _CHESS960==0
    // Standard chess castling
    if (board[0][0] == _WR)
    if (board[0][1] == 0)
    if (board[0][2] == 0)
    if (board[0][3] == 0)
    if (board[8][0] == 1) {
        copy_board(board, aux);
        aux[0][2] = _WK;
        aux[0][3] = _WK;
        if (! in_check(aux))
            addm(0, 4, 0, 2, curr_index, movelist);
    }
    if (board[0][7] == _WR)
    if (board[0][6] == 0)
    if (board[0][5] == 0)
    if (board[8][1] == 1) {
        copy_board(board, aux);
        aux[0][5] = _WK;
        aux[0][6] = _WK;
        if (! in_check(aux))
            addm(0, 4, 0, 6, curr_index, movelist);
    }
#else
    // Chess960 castling logic
    s5 i, rook_x;
    
    // Queenside castling (left)
    for (rook_x = 0; rook_x < 4; rook_x++) {
        if (board[0][rook_x] == _WR && board[8][0] == 1) {
            // Check if all squares between king and rook are empty
            int empty = 1;
            for (i = rook_x + 1; i < 4; i++) {
                if (board[0][i] != 0) {
                    empty = 0;
                    break;
                }
            }
            if (empty) {
                copy_board(board, aux);
                aux[0][2] = _WK;  // King moves to c1
                aux[0][3] = _WK;  // Rook moves to d1
                if (! in_check(aux))
                    addm(0, 4, 0, 2, curr_index, movelist);
            }
        }
    }
    
    // Kingside castling (right)
    for (rook_x = 5; rook_x < 8; rook_x++) {
        if (board[0][rook_x] == _WR && board[8][1] == 1) {
            // Check if all squares between king and rook are empty
            int empty = 1;
            for (i = 5; i < rook_x; i++) {
                if (board[0][i] != 0) {
                    empty = 0;
                    break;
                }
            }
            if (empty) {
                copy_board(board, aux);
                aux[0][5] = _WK;  // King moves to f1
                aux[0][6] = _WK;  // Rook moves to g1
                if (! in_check(aux))
                    addm(0, 4, 0, 6, curr_index, movelist);
            }
        }
    }
#endif
}

void genP(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    if (board[y + 1][x] == 0) {
        if (y != 6)
            addm(y, x, y + 1, x, curr_index, movelist);
        if (y == 6) {
            s5 to;
            for (to = 5; to >= 2; to--)
                addprom(y, x, y + 1, x, to, curr_index, movelist);
        }
        if (y == 1)
        if (board[y + 2][x] == 0)
            addm(y, x, y + 2, x, curr_index, movelist);
    }
    if (x > 0)
    if (board[y + 1][x - 1] < 0) {
        if (y != 6)
            addm(y, x, y + 1, x - 1, curr_index, movelist);
        else {
            s5 to;
            for (to = 5; to >= 2; to--)
                addprom(y, x, y + 1, x - 1, to, curr_index, movelist);
        }
    }
    if (x < 7)
    if (board[y + 1][x + 1] < 0) {
        if (y != 6)
            addm(y, x, y + 1, x + 1, curr_index, movelist);
        else {
            s5 to;
            for (to = 5; to >= 2; to--)
                addprom(y, x, y + 1, x + 1, to, curr_index, movelist);
        }
    }
}

void genN(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    nonslider(board, y, x, +2, -1, curr_index, movelist);
    nonslider(board, y, x, +2, +1, curr_index, movelist);
    nonslider(board, y, x, +1, +2, curr_index, movelist);
    nonslider(board, y, x, +1, -2, curr_index, movelist);
    nonslider(board, y, x, -1, +2, curr_index, movelist);
    nonslider(board, y, x, -1, -2, curr_index, movelist);
    nonslider(board, y, x, -2, -1, curr_index, movelist);
    nonslider(board, y, x, -2, +1, curr_index, movelist);
}

void genB(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    slider(board, y, x, +1, -1, curr_index, movelist);
    slider(board, y, x, +1, +1, curr_index, movelist);
    slider(board, y, x, -1, +1, curr_index, movelist);
    slider(board, y, x, -1, -1, curr_index, movelist);
}

void genR(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    slider(board, y, x, +1,  0, curr_index, movelist);
    slider(board, y, x,  0, -1, curr_index, movelist);
    slider(board, y, x,  0, +1, curr_index, movelist);
    slider(board, y, x, -1,  0, curr_index, movelist);
}

void genQ(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
{
    genR(board, y, x, curr_index, movelist);
    genB(board, y, x, curr_index, movelist);
}

void genK(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist, LEVEL depth)
{
    nonslider(board, y, x, -1, -1, curr_index, movelist);
    nonslider(board, y, x, -1,  0, curr_index, movelist);
    nonslider(board, y, x, -1, +1, curr_index, movelist);
    nonslider(board, y, x,  0, -1, curr_index, movelist);
    nonslider(board, y, x,  0, +1, curr_index, movelist);
    nonslider(board, y, x, +1, -1, curr_index, movelist);
    nonslider(board, y, x, +1,  0, curr_index, movelist);
    nonslider(board, y, x, +1, +1, curr_index, movelist);
#ifdef _ALLOW_CASTLE
    if (depth == 1)
        castle(board, y, x, curr_index, movelist);
#endif
}

MOVEINDEX gendeep(BOARD board, MOVELIST movelist, LEVEL depth)
{
    MOVEINDEX curr_index = 0;
    s5 x;
    s5 y;
    for (y = 7; y >= 0; y--)
    for (x = 0; x < 8; x++) {
        switch(board[y][x]) {
        case _WP: genP(board, y, x, &curr_index, movelist); break;
        case _WN: genN(board, y, x, &curr_index, movelist); break;
        case _WB: genB(board, y, x, &curr_index, movelist); break;
        case _WR: genR(board, y, x, &curr_index, movelist); break;
        case _WQ:
#ifdef _Q0BLK
            if (glevel)
#endif
               genQ(board, y, x, &curr_index, movelist); break;
        case _WK: genK(board, y, x, &curr_index, movelist, depth); break;
        default:;
        }
    }
    return (curr_index);
}

void nonslider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist)
{
    if (y + dy < 0) return;
    if (y + dy > 7) return;
    if (x + dx < 0) return;
    if (x + dx > 7) return;
    if (board[y + dy][x + dx] <= 0)
        addm(y, x, y + dy, x + dx, curr_index, movelist);
}

void slider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist)
{
    s5 y1 = y;
    s5 x1 = x;
    while (1) {
        y1 += dy;
        x1 += dx;
        if (y1 < 0) return;
        if (y1 > 7) return;
        if (x1 < 0) return;
        if (x1 > 7) return;
        if (board[y1][x1] < 0) {
            addm(y, x, y1, x1, curr_index, movelist);
            return;
        }
        if (board[y1][x1] > 0)
            return;
        addm(y, x, y1, x1, curr_index, movelist);
    }
}

void init(ELAPSED *elapsed)
{
    elapsed->seconds = 0;
    elapsed->useconds = 0;
    elapsed->clock0 = clock();
    elapsed->clock1 = clock();
    elapsed->diffclock = 0;
}

void update(ELAPSED *elapsed)
{
    elapsed->clock1 = clock();
    elapsed->diffclock = (elapsed->clock1 - elapsed->clock0);
    if (elapsed->diffclock < 0)
        elapsed->diffclock =0;
    if (elapsed->diffclock > 655360000)
        elapsed->diffclock = 0;
    elapsed->useconds += elapsed->diffclock;
    elapsed->seconds += (elapsed->useconds / 1000000);
    elapsed->useconds = elapsed->useconds % 1000000;
    elapsed->clock0 = clock();
}

double dclock(ELAPSED *elapsed)
{
    return (double) (elapsed->seconds) + (double) (elapsed->useconds) / 1000000.0;
}

int board_cmp(BOARD src, BOARD dest)
{
    u5 x;
    u5 y;
    for (y = 0; y < 9; y++)
    for (x = 0; x < 8; x++)
    if (dest[y][x] != src[y][x])
        return (1);
    return (0);
}

void copy_board(BOARD src, BOARD dest)
{
    u5 x;
    u5 y;
    for (y = 0; y < 9; y++)
    for (x = 0; x < 8; x++)
        dest[y][x] = src[y][x];
}

void copy_move(MOVE src, MOVE dest)
{
    u5 u;
    for (u = 0; u < 6; u++)
        dest[u] = src[u];
}

BOARD *get_init(void)
{
    static BOARD init = {
        {_WR, _WN, _WB, _WQ, _WK, _WB, _WN, _WR, },
        {_WP, _WP, _WP, _WP, _WP, _WP, _WP, _WP, },
        {_UO, _UO, _UO, _UO, _UO, _UO, _UO, _UO, }, 
        {_UO, _UO, _UO, _UO, _UO, _UO, _UO, _UO, }, 
        {_UO, _UO, _UO, _UO, _UO, _UO, _UO, _UO, }, 
        {_UO, _UO, _UO, _UO, _UO, _UO, _UO, _UO, }, 
        {_BP, _BP, _BP, _BP, _BP, _BP, _BP, _BP, },
        {_BR, _BN, _BB, _BQ, _BK, _BB, _BN, _BR, },
        { 1, 1, 1, 1, 0, 0, 0, 0, },
    };
    return (&init);
}

void load(BOARD board)
{
    FILE *f;
    s5 pp;
    u5 x;
    u5 y;
    f = fopen(_BRDFILE, "r");
    if (!f)
        warn("Cannot open .brd file for read");
    for (y = 8; y > 0; y--)
    for (x = 0; x < 8; x++) {
        fscanf(f, "%d", &pp);
        board[y - 1][x] = (s3) pp;
    }
    for (x = 0; x < 8; x++)
        board[8][x] = (x < 4);
    fscanf(f, "%d", &stm);
    fclose(f);
    show_board(board, stdout);
    if (stm)
        transpose(board);
}

void save(BOARD board)
{
    FILE *f;
    s5 pp;
    s5 x;
    s5 y;
    f = fopen(_BRDFILE, "w");
    if (!f)
        warn("Cannot open .brd file for write");
    for (y = 8; y > 0; y--) {
    for (x = 0; x < 8; x++) {
	pp = (s5) board[y - 1][x];
	fprintf(f, "%2d ", pp);
    }
	fprintf(f, "\n");
    }
    fprintf(f, "%d\n", stm);
    fflush(stdout);
    fclose(f);
    if (stm)
        transpose(board);
}

s4 in_check(BOARD board)
{
    BOARD aux;
    MOVE curr_move;
    MOVEINDEX curr_index;
    MOVEINDEX max_index;
    MOVELIST movelist;
    copy_board(board, aux);
    transpose(aux);
    max_index = gendeep(aux, movelist, 0);
    for (curr_index = 0; curr_index < max_index; curr_index++) {
        copy_move(movelist[curr_index], curr_move);
        if (aux[(u5) curr_move[2]][(u5) curr_move[3]] == _BK)
        return (1);
    }
    return (0);
}

void makemove(BOARD src, MOVE move, BOARD dest)
{
    copy_board(src, dest);
    if (dest[(u5) move[0]][(u5) move[1]] == _WK) {
        if (move[0] == 0)
        if (move[2] == 0)
        if (move[1] == 4) {
        if (move[3] == 2) {
            dest[0][0] = 0;
            dest[0][3] = _WR;
        }
        if (move[3] == 6) {
            dest[0][7] = 0;
            dest[0][5] = _WR;
        }
        }
        dest[8][0] = 0;
        dest[8][1] = 0;
    }
    if (dest[(u5) move[0]][(u5) move[1]] == _WR)
    if (move[0] == 0) {
        if (move[1] == 0)
        dest[8][0] = 0;
        if (move[1] == 7)
        dest[8][1] = 0;
    }
    if (dest[(u5) move[0]][(u5) move[1]] == _WP)
    if (move[0] == 6) {
        dest[(u5) move[2]][(u5) move[3]] = move[4] ? (u5) move[4] : _WQ;
        dest[(u5) move[0]][(u5) move[1]] = 0;
        transpose(dest);
        goto end;
    }
    if (dest[(u5) move[0]][(u5) move[1]] == _WP)
    if (move[0] == 4)
    if (move[1] != move[3])
    if (dest[(u5) move[0]][(u5) move[3]] == _BP)
        dest[(u5) move[0]][(u5) move[3]] = 0;
    dest[(u5) move[2]][(u5) move[3]] = dest[(u5) move[0]][(u5) move[1]];
    dest[(u5) move[0]][(u5) move[1]] = 0;
    transpose(dest);
end:    ;
}

s4 move_cmp(MOVE src, MOVE dest)
{
    u5 u;
    for (u = 0; u < 4; u++)
    if (src[u] != dest[u])
        return (u + 1);
    return (0);
}

void show_move(MOVE move, BOARD board, u5 stm, char *buf)
/*
 * FIXME
 * What about promotions?!
 */
{
    BOARD aux;
    char *p;
    p = buf;
    switch (board[(u5) move[0]][(u5) move[1]]) {
    case 2: p += sprintf(p, "N"); break;
    case 3: p += sprintf(p, "B"); break;
    case 4: p += sprintf(p, "R"); break;
    case 5: p += sprintf(p, "Q"); break;
    case 6: if ((move[1] == 4) && ((move[3] == 6) || (move[3] == 2))) {
        switch (move[3]) {
        case 6: p += sprintf(p, "O-O"); break;
        case 2: p += sprintf(p, "O-O-O"); break;
        default:;
        }
        return;
    } else p += sprintf(p, "K"); break;
    default:;
    }
    p += sprintf(p, "%c", 97 + move[1]);
    if (stm)
        p += sprintf(p, "%d", 9 - (move[0] + 1));
    else
        p += sprintf(p, "%d", move[0] + 1);
    if (((board[(u5) move[0]][(u5) move[1]] == 1) && \
        (move[1] != move[3])) || \
        ((board[(u5) move[0]][(u5) move[1]] > 1) && \
        (board[(u5) move[2]][(u5) move[3]] < 0)))
        p += sprintf(p, "x");
    p += sprintf(p, "%c", 97 + move[3]);
    if (stm)
        p += sprintf(p, "%d", 9 - (move[2] + 1));
    else
        p += sprintf(p, "%d", move[2] + 1);
    makemove(board, move, aux);
    if (in_check(aux))
        p += sprintf(p, "+");
    (*p) = 0;
}

void show_board(BOARD board, FILE *f)
{
    s3 piece_symbol[80];
    wchar_t c;
    s3 s;
    s3 t;
    u5 x;
    u5 y;
    for (y = 0; y < 8; y++) {
        fprintf(f, "%d", (int) (8 - y));
    for (x = 0; x < 8; x++) {
        t = board[7 - y][x];
	s5 bg = 4 - 3 * ((x ^ y) & 1);
	s5 fg = 7 * (t > 0);
	switch (t) {
		case 0: fprintf(f, " . "); break;
		case 1: fprintf(f, " P "); break;
		case 2: fprintf(f, " N "); break;
		case 3: fprintf(f, " B "); break;
		case 4: fprintf(f, " R "); break;
		case 5: fprintf(f, " Q "); break;
		case 6: fprintf(f, " K "); break;
		case -1: fprintf(f, " p "); break;
		case -2: fprintf(f, " n "); break;
		case -3: fprintf(f, " b "); break;
		case -4: fprintf(f, " r "); break;
		case -5: fprintf(f, " q "); break;
		case -6: fprintf(f, " k "); break;
		default: warn("Wrong piece code");
	}
    }
        fprintf(f, "\n");
    }
    fprintf(f, "  a  b  c  d  e  f  g  h\n");
    fprintf(f, "\n");
    fflush(f);
}

void transpose(BOARD board)
{
    s3 t;
    u5 x;
    u5 y;
    for (y = 0; y < 4; y++)
    for (x = 0; x < 8; x++) {
        t = board[y][x];
        board[y][x] = board[7 - y][x];
        board[7 - y][x] = t;
    }
    for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++)
        board[y][x] = -board[y][x];
    t = board[8][0];
    board[8][0] = board[8][2];
    board[8][2] = t;
    t = board[8][1];
    board[8][1] = board[8][3];
    board[8][3] = t;
}

void setup_board(BOARD board)
{
	s5 x = 3;
	s5 y = 3;
	unsigned int symbol = 0;
	parse_fen(board);
	while (1) {
		printf("\033[2J");
		printf("\033[1;1H");
		show_board(board, stdout);
		system("cat start.fen");
		printf("enter square %d %c%d\n",
			stm, x + 97, y + 1);
		fflush(stdout);
		scanf("%u", &symbol);
		fflush(stdin);
		switch(symbol) {
			case 4: 
				return;
				break;
			case 20: board[y][x] = 0; break;
			case 31: 
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
				board[y][x] = symbol - 30;
				break;
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
				board[y][x] = 40 - symbol;
				break;
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
				x = symbol - 51;
				break;
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:
			case 68:
				y = symbol - 61;
				break;
			case 70:
				copy_board(*get_init(), board);
				break;
			case 71:
				load(board);
				break;
			case 72:
				save(board);
				break;
			case 73:
				stm = 1 - stm;
				board[8][4] = stm;
				break;
			case 74:
				exit(0);
				break;
			case 75:
				transpose(board);
				break;
			case 76:
				parse_fen(board);
				break;
			default:;
		}
	}
}

void load_values(void)
{
	FILE *vf = fopen("start.bpf", "r");
	if (!vf)
		exit(1);
	for (int i = 1; i < 6; i++) {
		int j;
		fscanf(vf, "%d", &j);
		if (j != i)
			exit(0);
		fscanf(vf, "%d", &_VALUES[i]);
	}
	_VALUES[0] = 0;
	fclose(vf);
}

int main(int argc, char *argv[])
{
    gmode = 4;
    srand(time(NULL));
    load_values();
    analysis();
    return (0);
}

void warn(const char *msg)
{
    fprintf(stderr, "\nwarn %s\n", msg);
}

void parse_fen(BOARD board) {
  FILE *f;
  int x = 1;
  int y = 8;
  int ch;
  int pp;
  f = fopen("start.fen", "r");
  if (!f)
    warn("warn in parse_fen"); // Make sure 'warn' is defined
  while ((ch = fgetc(f)) != EOF && ch != ' ') {
    pp = 0;
    if ((ch >= '1') && (ch <= '8')) {
      while (ch > '0') {
        board[y - 1][x - 1] = 0;
        x++;
        ch--;
      }
      if (x == 9) {
        x = 1;
        y--;
        if (!y) {
	  ch = fgetc(f);
          goto end;
	}
        continue;
      }
    } else if (ch != '/') {
      switch (ch) {
        case 'p': pp = -1; break;
        case 'n': pp = -2; break;
        case 'b': pp = -3; break;
        case 'r': pp = -4; break;
        case 'q': pp = -5; break;
        case 'k': pp = -6; break;
        case 'P': pp = 1; break;
        case 'N': pp = 2; break;
        case 'B': pp = 3; break;
        case 'R': pp = 4; break;
        case 'Q': pp = 5; break;
        case 'K': pp = 6; break;
        default: pp = -8;
      }
      if (pp > -8)
        board[y - 1][x - 1] = pp;
      x++;
      if (x == 9) {
        x = 1;
        y--;
        if (!y) {
          ch = fgetc(f);
          goto end;
        }
      }
    }
  }

end:
  ch = fgetc(f);
  fclose(f);
  // Update stm based on the character read from FEN
  if (ch == 'w')
    stm = 0;
  else if (ch == 'b')
    stm = 1;
  else {
      warn("Cannot set `stm' variable");
  }
}

void parse_pgn(void)
{
    if (system("pgn-extract -F start.pgn -w200 > pf") != 0) {
        // Handle error when pgn-extract fails
        fprintf(stderr, "Error running pgn-extract\n");
        return;
    }
    FILE *f = fopen("pf", "r");
    FILE *g = fopen("start.fen", "w");
    if (!f || !g) {
        fprintf(stderr, "Error opening files\n");
        if (f) fclose(f);
        if (g) fclose(g);
        return;
    }
    int ch;
    BOARD board;
    copy_board(*get_init(), board);
    while ((ch = fgetc(f)) != '{') {}
    while ((ch = fgetc(f)) != '"') {}
    while ((ch = fgetc(f)) != '"') fputc(ch, g);
    fclose(f);
    fclose(g);
    parse_fen(board);
    save(board);
}
