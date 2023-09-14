#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define _ALLOW_CASTLE (1)
#define _C_DEPTH (4)
#define _DEBUG (1)
#define _GAME_LOST (800)
#ifndef _MAXINDEX
#define _MAXINDEX (200)
#endif
#define _MAXLEVEL (40)
#define _FRAMESPERSEC (50)
#define _NPS (70000)
#define _SKIPFRAMES (_NPS / _FRAMESPERSEC)
#define _DIR "/sdcard/Files/projects/baeagn"
#define _ERRORFILE "baeagn.err"
#define _BRDFILE "start.brd"

#define _CUSTOM (0)

#if (_CUSTOM == 0)
#define _FACTORY (1)
#define _ALPHA (-20000)
#define _BETA (20000)
#define _OVERDEPTH (1)
#define _S_DEPTH (3)
#define _SORT
#undef _CAND7
#undef _CAND250
#undef _CANDCUT (450)
#undef _Q0BLK
#endif

#if (_CUSTOM == 1)
#define _SAHB (1)
#endif

#ifdef _SAHB
#define _ALPHA (-20000)
#define _BETA (20000)
#define _OVERDEPTH (1)
#define _S_DEPTH (3)
#define _SORT
#undef _CAND7
#undef _CAND250
#undef _CANDCUT (450)
#undef _Q0BLK
#endif

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

typedef signed char s3;
typedef signed short int s4;
typedef signed int s5;
typedef signed long long s6;
typedef unsigned char u3;
typedef unsigned short int u4;
typedef unsigned int u5;
typedef unsigned long long u6;

typedef u5 NODES;
typedef double TIME;
typedef s3 MOVE[6];
typedef MOVE MOVELIST[_MAXINDEX];
typedef s3 BOARD[9][8];
typedef s4 VALUE;
typedef u4 LEVEL;
typedef u4 MOVEINDEX;

typedef struct {
    int seconds;
    int cseconds;
    clock_t clock0;
    clock_t clock1;
    int diffclock;
} ELAPSED;

void init(ELAPSED *elapsed)
{
    elapsed->seconds = 0;
    elapsed->cseconds = 0;
    elapsed->clock0 = clock();
    elapsed->clock1 = clock();
    elapsed->diffclock = 0;
}

void update(ELAPSED *elapsed)
{
    elapsed->clock1 = clock();
    elapsed->diffclock = (int) (elapsed->clock1 - elapsed->clock0) / 10000;
    if (elapsed->diffclock < 0)
        elapsed->diffclock =0;
    if (elapsed->diffclock > 65536)
        elapsed->diffclock = 0;
    elapsed->cseconds += elapsed->diffclock;
    elapsed->seconds += (elapsed->cseconds / 100);
    elapsed->cseconds = elapsed->cseconds % 100;
    elapsed->clock0 = clock();
}

double dclock(ELAPSED *elapsed)
{
    return (double) (elapsed->seconds) + (double) \
        (elapsed->cseconds) / 100.0;
}

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
} LVLCTX;

const VALUE _ALPHA_DFL    = (-20000);
const VALUE _BETA_DFL     = (+20000);
const VALUE _MAXVALUE     = (20000);
const VALUE _PAWNUNIT     = (100);
const VALUE _THRESHOLD    = (15000);
const VALUE _VALUES[6]    = { 0, 100, 315, 325, 500, 980, };
const LEVEL _MYLEVELS[18] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 20, 24, 32, 40, 48, 64, 0, };

time_t time0;
clock_t clock0;
ELAPSED elapsed;
LEVEL gdepth;
LEVEL glevel;
LVLCTX *ctxa;
NODES nodes;
s4 gmode;
s4 stm;

void addm(s5 y, s5 x, s5 y1, s5 x1, MOVEINDEX *curr_index, MOVELIST movelist);
void addprom(s5 y, s5 x, s5 y1, s5 x1, s5 to, MOVEINDEX *curr_index, MOVELIST movelist);
void analysis(void);
VALUE backtrack(LVLCTX *ctxa, LEVEL level, LEVEL depth);
int board_cmp(BOARD src, BOARD dest);
void copy_board(BOARD src, BOARD dest);
void copy_move(MOVE src, MOVE dest);
void castle(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
void error(const char *msg);
VALUE eval(BOARD board, LEVEL level);
void game_hh(void);
void game_ch(void);
void game_hc(void);
void game_cc(void);
void genP(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
void genN(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
void genB(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
void genR(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
void genQ(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist);
void genK(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist, LEVEL depth);
MOVEINDEX gendeep(BOARD board, MOVELIST movelist, LEVEL depth);
MOVEINDEX gen(BOARD board, MOVELIST movelist, LEVEL level);
BOARD *get_init(void);
void load(BOARD start);
s4 in_check(BOARD board);
s4 is_pv(LEVEL level);
void makemove(BOARD src, MOVE move, BOARD dest);
s4 move_cmp(MOVE src, MOVE dest);
void nonslider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist);
void play_hw(BOARD board, s4 ply);
void play_hb(BOARD board, s4 ply);
void play_cw(BOARD board, s4 ply);
void play_cb(BOARD board, s4 ply);
void show_move(MOVE move, BOARD board, u5 stm, char *buf);
void slider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist);
void show_board(BOARD board, FILE *f);
void transpose(BOARD board);

void addm(s5 y, s5 x, s5 y1, s5 x1, MOVEINDEX *curr_index, MOVELIST movelist)
{
    movelist[*curr_index][0] = y;
    movelist[*curr_index][1] = x;
    movelist[*curr_index][2] = y1;
    movelist[*curr_index][3] = x1;
    (*curr_index)++;
    if (*curr_index >= _MAXINDEX)
        error("Index too big");
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
        error("Index too big");
}

void analysis(void)
{
    BOARD aux;
    BOARD aux2;
    BOARD start;
    char buf[80];
    LEVEL depth;
    LEVEL i;
    LVLCTX *ctx;
    s4 ix = 0;
    //system("/system/bin/cp main2 /sdcard/");
    chdir(_DIR);
    load(start);
    show_board(start, stdout);
    ctxa = (LVLCTX *) malloc (_MAXLEVEL * sizeof(LVLCTX));
    if (!ctxa) {
        error("Out of memory!");
    }
    init(&elapsed);
    time0 = time(NULL);
    clock0 = clock();
    nodes = 0;
    for (depth = _MYLEVELS[ix]; ix < 16; depth = _MYLEVELS[++ix]) {
        ctx = &ctxa[0];
        copy_board(start, ctx->curr_board);
        ctx->level = 0;
        ctx->depth = depth + _OVERDEPTH;
        gdepth = ctx->depth;
        ctx->alpha = _ALPHA;
        ctx->beta = _BETA;
        ctx->best = backtrack(ctxa, 0, 1);
        update(&elapsed);
        double delapsed = (double) (clock() - clock0) / CLOCKS_PER_SEC;
        copy_board(start, aux);
        fprintf(stdout, "Depth: %u\n", depth);
        fprintf(stdout, "Evaluation: %.2lf\n", \
            ((double) (ctx->best) / (double) _PAWNUNIT));
        fprintf(stdout, "Branch factor: %.2lf\n", pow((double) nodes, (double) 1 / (depth)));
        fprintf(stdout, "Best variation: ");
        for (i = 0; i < ctx->bl_len; i++) {
            show_move(ctx->best_line[i], aux, (i + stm) % 2, buf);
            makemove(aux, ctx->best_line[i], aux2);
            copy_board(aux2, aux);
            fprintf(stdout, "%s ", buf);
        }
        fprintf(stdout, "\n");
        if (ctx->bl_len & 1)
            transpose(aux);
        fprintf(stdout, "Elapsed: %.2lf\n", delapsed);
        fprintf(stdout, "kNPS: %.2lf\n", (0.001 * (double) nodes / delapsed));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    free(ctxa);
}

VALUE backtrack(LVLCTX *ctxa, LEVEL level, LEVEL depth)
// level means distance from root
// depth means 1 if ctxa, 0 if ctxb
{
    BOARD aux;
    BOARD aux2;
    char buf[80];
    LEVEL bl_lev;
    LEVEL i;
    LVLCTX *ctx;
    LVLCTX *nctx;
    VALUE value;
    ctx = &ctxa[level];
    ctx->bl_len = 0;
    value = eval(ctx->curr_board, level);
    if (value < -_THRESHOLD)
        return (value);
    if (ctx->depth == 0)
        return (value);
    if (ctx->depth <= _OVERDEPTH)
    if (value > -(_PAWNUNIT >> 1))
        return (value);
    if (depth)
        glevel = level;
    ctx->max_index = gen(ctx->curr_board, ctx->legal_moves, depth);
    if (ctx->max_index == 0)
        return (-_MAXVALUE + level);
    ctx->best = -_MAXVALUE;
    ctx->bl_len = 1;
    for (ctx->curr_index = 0; ctx->curr_index < ctx->max_index; (ctx->curr_index)++) {
        nctx = &ctxa[level + 1];
        copy_move(ctx->legal_moves[ctx->curr_index], ctx->curr_move);
        makemove(ctx->curr_board, ctx->curr_move, ctx->next_board);
        copy_board(ctx->next_board, nctx->curr_board);
        nctx->level = ctx->level + 1;
        nctx->depth = ctx->depth - 1;
        nctx->alpha = -(ctx->beta);
        nctx->beta = -(ctx->alpha);
        ctx->value = -backtrack(ctxa, level + 1, depth);
        if (ctx->value > ctx->best) {
            ctx->best = ctx->value;
            ctx->bl_len = nctx->bl_len + 1;
            copy_move(ctx->curr_move, ctx->best_line[0]);
            if (nctx->bl_len > 0)
            for (bl_lev = 0; bl_lev < nctx->bl_len; bl_lev++)
                copy_move(nctx->best_line[bl_lev], \
                    ctx->best_line[bl_lev + 1]);
            if (level == 0 && depth == 1 && gmode == 4) {
                update(&elapsed);
                double delapsed = difftime(time(NULL), time0);
                copy_board(ctx->curr_board, aux);
                fprintf(stdout, "Depth: %u*\n", ctx->depth - _OVERDEPTH);
                fprintf(stdout, "Evaluation: %.2lf\n", \
                    ((double) ctx->best / (double) _PAWNUNIT));
                fprintf(stdout, "Branch factor: %.2lf\n", pow((double) nodes, (double) 1 / (ctx->depth - _OVERDEPTH)));
                fprintf(stdout, "Best variation: ");
                for (i = 0; i < ctx->bl_len; i++) {
                    show_move(ctx->best_line[i], aux, (i + stm) % 2, buf);
                    makemove(aux, ctx->best_line[i], aux2);
                    copy_board(aux2, aux);
                    fprintf(stdout, "%s ", buf);
                }
                fprintf(stdout, "\n");
                if (ctx->bl_len & 1)
                    transpose(aux);
                fprintf(stdout, "Elapsed: %.2lf\n", delapsed);
                fprintf(stdout, "Nodes: %d\n", nodes);
                fprintf(stdout, "\n");
                fflush(stdout);
            }
            if (ctx->best > ctx->alpha)
                ctx->alpha = ctx->best;
            if (ctx->alpha >= ctx->beta)
                return (ctx->beta);
        }
    }
    return (ctx->best);
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

void castle(BOARD board, s5 y, s5 x, MOVEINDEX *curr_index, MOVELIST movelist)
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
}

void error(const char *msg)
{
    FILE *f;
    f = fopen(_ERRORFILE, "w");
    if (!f) {
        fprintf(stderr, "Cannot open error file %s\n", _ERRORFILE);
        exit(0);
    }
    fprintf(f, "\nerror %s\n", msg);
    fclose(f);
    exit(0);
}

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
    VALUE value = 0;
    nodes++;
    if ((nodes % _SKIPFRAMES) == 0) {
        update(&elapsed);
    }
    for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++) {
        switch (board[y][x]) {
        case _WP:
            switch (y) {
            case 1:
            case 2:
            case 3:
            case 4:
                ivalue += 100;
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
            case 3:
                ivalue -= 100;
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
#ifdef _SAHB
    if (board[2][3] == _WP)
        ivalue -= 434;
    if (board[5][3] == _BP)
        ivalue += 434;
    if (board[3][3] == _WP)
    if (board[4][4] != _BP)
        ivalue += 12;
    if (board[4][3] == _BP)
    if (board[3][4] != _WP)
        ivalue -= 12;
#if 1
    for (y = 0; y < 8; y++) {
        if (board[y][3] == _WP)
            break;
        if (board[y][3] == _BP)
            break;
        if (y == 7) {
        if ((level ^ stm) % 2)
            ivalue += 230;
        else
            ivalue -= 230;
        }
    }
#endif
#endif
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
    if (ctxa[level].depth == 1) {
        copy_board(board, aux);
        transpose(aux);
        if (in_check(aux))
            return (_MAXVALUE - (level + 1));
    }
    if (ctxa[level].depth / 2 == 1)
    if (in_check(board))
        return (-2000 + value + level);
    value += ((rand() % 19) - 9);
    if (level > 1)
        return (value + (ctxa[level - 2].max_index - ctxa[level - 1].max_index));
    return (value);
}

void game_hh(void)
{
    BOARD start;
    s4 ply;
    copy_board(*get_init(), start);
    srand(time(NULL));
    for (ply = 1; ply < 32767; ply++) {
        play_hw(start, ply);
        play_hb(start, ply);
    }
}

void game_ch(void)
{
    BOARD start;
    s4 ply;
    copy_board(*get_init(), start);
    srand(time(NULL));
    for (ply = 1; ply < 32767; ply++) {
        play_cw(start, ply);
        play_hb(start, ply);
    }
}

void game_hc(void)
{
    BOARD start;
    s4 ply;
    copy_board(*get_init(), start);
    srand(time(NULL));
    for (ply = 1; ply < 32767; ply++) {
        play_hw(start, ply);
        play_cb(start, ply);
    }
}

void game_cc(void)
{
    BOARD start;
    s4 ply;
	load(start);
    srand(time(NULL));
    for (ply = 1; ply < 32767; ply++) {
        play_cw(start, ply);
        play_cb(start, ply);
    }
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

MOVEINDEX gen(BOARD board, MOVELIST movelist, LEVEL depth)
// depth means 1 if sortable, 0 otherwise
// FIXME
{
    MOVEINDEX max_index = gendeep(board, movelist, 1);
#ifdef _SORT
    if (depth == 1 && glevel < gdepth - _S_DEPTH -1) {
    LVLCTX *ctxb;
    MOVEINDEX curr_index;
    MOVEINDEX ncurr_index;
    VALUE valuelist[_MAXINDEX];
    ctxb = (LVLCTX *) malloc(_MAXLEVEL * sizeof(LVLCTX));
    if (!ctxb)
        error("Out of memory");
    for (curr_index = 0; curr_index < max_index; curr_index++) {
        BOARD aux;
        MOVE move;
        copy_move(movelist[curr_index], move);
        makemove(board, move, aux);
        copy_board(aux, ctxb[0].curr_board);
        ctxb[0].level = 0;
        ctxb[0].depth = _S_DEPTH;
        ctxb[0].alpha = _ALPHA_DFL;
        ctxb[0].best = _BETA_DFL;
        valuelist[curr_index] = -backtrack(ctxb, 0, 0);
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
    free(ctxb);
#ifdef _CAND7
    LEVEL newmax_index = max_index;
    if (glevel)
        newmax_index = 7;
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
    s5 pp;
    u5 x;
    u5 y;
    for (y = 8; y > 0; y--)
    for (x = 0; x < 8; x++) {
        fscanf(stdin, "%d", &pp);
        board[y - 1][x] = (s3) pp;
    }
    for (x = 0; x < 8; x++)
        board[8][x] = (x < 4);
    fscanf(stdin, "%d", &stm);
    show_board(board, stdout);
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

void nonslider(BOARD board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX *curr_index, MOVELIST movelist)
{
    if (y + dy < 0) return;
    if (y + dy > 7) return;
    if (x + dx < 0) return;
    if (x + dx > 7) return;
    if (board[y + dy][x + dx] <= 0)
        addm(y, x, y + dy, x + dx, curr_index, movelist);
}

void play_hw(BOARD start, s4 ply)
{
    BOARD aux;
    char buf[80];
    MOVEINDEX cnt_index;
    MOVEINDEX curr_index;
    MOVEINDEX max_index;
    MOVELIST movelist;
    MOVE move;
again:
    printf("Legal moves: ");
    max_index = gen(start, movelist, 0);
    cnt_index = 0;
    for (curr_index = 0; curr_index < max_index; curr_index++) {
        makemove(start, movelist[curr_index], aux);
        transpose(aux);
        if (in_check(aux))
            continue;
        buf[0] = movelist[curr_index][1] + 97;
        buf[1] = movelist[curr_index][0] + 49;
        buf[2] = movelist[curr_index][3] + 97;
        buf[3] = movelist[curr_index][2] + 49;
        buf[4] = 0;
        printf("%s ", buf);
        cnt_index++;
    }
    if (cnt_index == 0) {
        if (in_check(start)) {
            printf("Checkmate.\n");
            printf("Black wins.\n");
            fflush(stdout);
            exit(0);
        } else {
            printf("Stalemate.\n");
            printf("Draw.\n");
            fflush(stdout);
            exit(0);
        }
    }
    printf("\nYour move? %d . ", ply);
    fflush(stdout);
    scanf("%s", buf);
    if (!strcmp(buf, "resign")) {
        printf("White resigns.\n");
        printf("Black wins.\n");
        fflush(stdout);
        exit(0);
    }
    move[0] = buf[1] - 49;
    move[1] = buf[0] - 97;
    move[2] = buf[3] - 49;
    move[3] = buf[2] - 97;
    /*
     * FIXME
     * What about promotions?!
     */
    move[4] = 0;
    for (curr_index = 0; curr_index < max_index; curr_index++) {
        if (!move_cmp(move, movelist[curr_index]))
            goto legal;
    }
    printf("Illegal move\n");
    fflush(stdout);
    goto again;
legal:
    makemove(start, move, aux);
    transpose(aux);
    if (in_check(aux)) {
        printf("Illegal move\n");
        fflush(stdout);
        goto again;
    }
    show_board(aux, stdout);
    transpose(aux);
    copy_board(aux, start);
}

void play_hb(BOARD start, s4 ply)
{
    BOARD aux;
    char buf[80];
    MOVEINDEX cnt_index;
    MOVEINDEX curr_index;
    MOVEINDEX max_index;
    MOVELIST movelist;
    MOVE move;
again:
    printf("Legal moves: ");
    max_index = gen(start, movelist, 0);
    cnt_index = 0;
    for (curr_index = 0; curr_index < max_index; curr_index++) {
        makemove(start, movelist[curr_index], aux);
        transpose(aux);
        if (in_check(aux))
            continue;
        buf[0] = movelist[curr_index][1] + 97;
        buf[1] = (7 ^ movelist[curr_index][0]) + 49;
        buf[2] = movelist[curr_index][3] + 97;
        buf[3] = (7 ^ movelist[curr_index][2]) + 49;
        buf[4] = 0;
        printf("%s ", buf);
        cnt_index++;
    }
    if (cnt_index == 0) {
        if (in_check(start)) {
            printf("Checkmate.\n");
            printf("White wins.\n");
            fflush(stdout);
            exit(0);
        } else {
            printf("Stalemate.\n");
            printf("Draw.\n");
            fflush(stdout);
            exit(0);
        }
    }
    printf("\nYour move? %d ... ", ply);
    fflush(stdout);
    scanf("%s", buf);
    if (!strcmp(buf, "resign")) {
        printf("Black resigns.\n");
        printf("White wins.\n");
        fflush(stdout);
        exit(0);
    }
    move[0] = 7 ^ (buf[1] - 49);
    move[1] = buf[0] - 97;
    move[2] = 7 ^ (buf[3] - 49);
    move[3] = buf[2] - 97;
    /*
     * FIXME
     * What about promotions?!
     */
    move[4] = 0;
    for (curr_index = 0; curr_index < max_index; curr_index++) {
        if (!move_cmp(move, movelist[curr_index]))
            goto legal;
    }
    printf("Illegal move\n");
    fflush(stdout);
    goto again;
legal:
    move[0] = 7 ^ (buf[1] - 49);
    move[1] = buf[0] - 97;
    move[2] = 7 ^ (buf[3] - 49);
    move[3] = buf[2] - 97;
    makemove(start, move, aux);
    if (in_check(aux)) {
        printf("Illegal move\n");
        fflush(stdout);
        goto again;
    }
    copy_board(aux, start);
    show_board(start, stdout);
}

void play_cw(BOARD start, s4 ply)
{
    BOARD aux;
    char buf[80];
    LEVEL depth;
    MOVE move;
    depth = _C_DEPTH;
    ctxa = (LVLCTX *) malloc ((depth + _OVERDEPTH + 1) * sizeof(LVLCTX));
    nodes = 0;
    copy_board(start, ctxa[0].curr_board);
    ctxa[0].level = 0;
    ctxa[0].depth = depth +_OVERDEPTH;
    ctxa[0].alpha = -_MAXVALUE;
    ctxa[0].beta = _MAXVALUE;
    ctxa[0].best = backtrack(ctxa, 0, 0);
    if (ctxa[0].best < -_GAME_LOST) {
        printf("White resigns.\n");
        printf("Black wins.\n");
        fflush(stdout);
        exit(0);
    }
    copy_move(ctxa[0].best_line[0], move);
    free(ctxa);
    show_move(move, start, 0, buf);
    printf("My move: %d . %s\n", ply, buf);
    fflush(stdout);
    makemove(start, move, aux);
    copy_board(aux, start);
    transpose(start);
    show_board(start, stdout);
    transpose(start);
}

void play_cb(BOARD start, s4 ply)
{
    BOARD aux;
    char buf[80];
    LEVEL depth;
    MOVE move;
    depth = _C_DEPTH;
    ctxa = (LVLCTX *) malloc ((depth + _OVERDEPTH + 1) * sizeof(LVLCTX));
    nodes = 0;
    copy_board(start, ctxa[0].curr_board);
    ctxa[0].level = 0;
    ctxa[0].depth = depth +_OVERDEPTH;
    ctxa[0].alpha = -_MAXVALUE;
    ctxa[0].beta = _MAXVALUE;
    ctxa[0].best = backtrack(ctxa, 0, 0);
    if (ctxa[0].best < -_GAME_LOST) {
        printf("Black resigns.\n");
        printf("White wins.\n");
        fflush(stdout);
        exit(0);
    }
    copy_move(ctxa[0].best_line[0], move);
    free(ctxa);
    show_move(move, start, 1, buf);
    printf("My move: %d ... %s\n", ply, buf);
    fflush(stdout);
    makemove(start, move, aux);
    copy_board(aux, start);
    show_board(start, stdout);
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

void show_board(BOARD board, FILE *f)
{
    s3 piece_symbol[80];
    s3 c;
    s3 s;
    s3 t;
    u5 x;
    u5 y;
    strcpy((char *) piece_symbol, " PNBRQK");
    for (y = 0; y < 8; y++) {
        fprintf(f, "%d", (int) (8 - y));
    for (x = 0; x < 8; x++) {
        t = board[7 - y][x];
        fprintf(f, "%3d", t);
    }
        fprintf(f, "\n");
    }
    fprintf(f, "   a  b  c  d  e  f  g  h \n");
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

int main(int argc, char *argv[])
{
    gmode = 4;
    analysis();
    return (0);
}
