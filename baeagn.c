#include "config.h"

const VALUE _ALPHA_DFL    = (-20000);
const VALUE _BETA_DFL     = (+20000);
const VALUE _MAXVALUE     = (20000);
const VALUE _PAWNUNIT     = (100);
const VALUE _THRESHOLD    = (15000);
const VALUE _VALUES[6]    = { 0, 100, 315, 325, 500, 980, };

ELAPSED elapsed;
LEVEL gdepth;
LEVEL glevel;
TREE *treea;
TREE *treeb;
MOVE best_move;
NODES nodes;
int newpv;
int pvsready;
s4 gmode;
s4 stm;

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
    TREE *tree;
    s4 ix = 0;
    load(start);
    show_board(start, stdout);
    treea = (TREE *) malloc (_MAXLEVEL * sizeof(TREE));
    if (!treea) {
        error("Out of memory!");
    }
    treeb = (TREE *) malloc(_MAXLEVEL * sizeof(TREE));
    if (!treeb)
        error("Out of memory");
    init(&elapsed);
    nodes = 0;
    pvsready = 0;
    for (depth = 1; depth < _MAXLEVEL; depth++) {
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
        fprintf(stdout, "\nDepth: %u\n", depth);
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
        fprintf(stdout, "kNPS: %.2lf\n", (0.001 * (double) nodes / delapsed));
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
                fprintf(stdout, "\nDepth: %u*\n", treea->depth - _OVERDEPTH);
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
                fprintf(stdout, "kNPS: %.2lf\n", (0.001 * (double) nodes / delapsed));
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
    VALUE value;
    nodes++;
    if ((nodes % _SKIPFRAMES) == 0) {
        update(&elapsed);
        char buf[80];
        MOVE move;
        copy_move(treea->curr_move, move);
        show_move(move, treea->curr_board, stm, buf);
        MOVEINDEX curr_index = treea->curr_index;
        MOVEINDEX max_index = treea->max_index;
        fprintf(stderr, "\r%d %d/%d %s\033[K\r", elapsed.seconds, curr_index + 1, max_index, buf);
        fflush(stderr);
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
    value += ((rand() % 19) - 9);
    if (level > 1)
        return (value + (treea[level - 2].max_index - treea[level - 1].max_index));
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
        newmax_index = 2;
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
    FILE *f;
    s5 pp;
    u5 x;
    u5 y;
    f = fopen(_BRDFILE, "r");
    if (!f)
        error("Cannot open .brd file for read");
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
    treea = (TREE *) malloc ((depth + _OVERDEPTH + 1) * sizeof(TREE));
    nodes = 0;
    copy_board(start, treea[0].curr_board);
    treea[0].level = 0;
    treea[0].depth = depth +_OVERDEPTH;
    treea[0].alpha = -_MAXVALUE;
    treea[0].beta = _MAXVALUE;
    treea[0].best = search(treea, 0, 0);
    if (treea[0].best < -_GAME_LOST) {
        printf("White resigns.\n");
        printf("Black wins.\n");
        fflush(stdout);
        exit(0);
    }
    copy_move(treea[0].best_line[0], move);
    free(treea);
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
    treea = (TREE *) malloc ((depth + _OVERDEPTH + 1) * sizeof(TREE));
    nodes = 0;
    copy_board(start, treea[0].curr_board);
    treea[0].level = 0;
    treea[0].depth = depth +_OVERDEPTH;
    treea[0].alpha = -_MAXVALUE;
    treea[0].beta = _MAXVALUE;
    treea[0].best = search(treea, 0, 0);
    if (treea[0].best < -_GAME_LOST) {
        printf("Black resigns.\n");
        printf("White wins.\n");
        fflush(stdout);
        exit(0);
    }
    copy_move(treea[0].best_line[0], move);
    free(treea);
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

