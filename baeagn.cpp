#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>
#include <iostream>

// Constants
constexpr int NOEDIT = 1;
constexpr int ALLOW_CASTLE = 1;
constexpr int DEBUG = 0;
constexpr int GAME_LOST = 800;
constexpr int MAXINDEX = 200;
constexpr int MAXLEVEL = 253;
constexpr int FRAMESPERSEC = 32;
constexpr int NPS = 3 << 18;
constexpr int SKIPFRAMES = NPS / FRAMESPERSEC;
constexpr const char* BRDFILE = "start.brd";
constexpr const char* FENFILE = "start.fen";

constexpr int ALPHA = -20000;
constexpr int BETA = 20000;
constexpr int OVERDEPTH = 1;
constexpr int S_DEPTH = 4;
constexpr int CANDCUT = 7500;

constexpr int WP = 1;
constexpr int WN = 2;
constexpr int WB = 3;
constexpr int WR = 4;
constexpr int WQ = 5;
constexpr int WK = 6;
constexpr int BP = -1;
constexpr int BN = -2;
constexpr int BB = -3;
constexpr int BR = -4;
constexpr int BQ = -5;
constexpr int BK = -6;
constexpr int WM = 7;
constexpr int BM = -7;
constexpr int UO = 0;
constexpr int BS = -8;

constexpr int ALPHA_DFL = -20000;
constexpr int BETA_DFL = 20000;
constexpr int MAXVALUE = 20000;
constexpr int PAWNUNIT = 100;
constexpr int THRESHOLD = 15000;

// Type aliases for clarity and compatibility
using s3 = int32_t;
using s4 = int32_t;
using s5 = int32_t;
using s6 = int64_t;
using u3 = uint32_t;
using u4 = uint32_t;
using u5 = uint32_t;
using u6 = uint64_t;
using NODES = u6;
using TIME = double;
using VALUE = s4;
using LEVEL = u4;
using MOVEINDEX = u4;

// Board and Move types
using Move = std::array<s3, 6>;
using MoveList = std::array<Move, MAXINDEX>;
using Board = std::array<std::array<s3, 8>, 9>;

// Piece-square tables
inline constexpr int pcsq[6][8][8] = {
    // Pawn
    {{0, 0, 0, 0, 0, 0, 0, 0}, {10, 10, 10, 10, 10, 10, 10, 10}, {25, 25, 25, 25, 25, 25, 25, 25},
     {35, 35, 35, 40, 40, 35, 35, 35}, {45, 45, 45, 50, 50, 45, 45, 45}, {60, 60, 60, 65, 65, 60, 60, 60},
     {80, 80, 80, 85, 85, 80, 80, 80}, {0, 0, 0, 0, 0, 0, 0, 0}},
    // Knight
    {{-50, -40, -30, -30, -30, -30, -40, -50}, {-40, -20, 0, 5, 5, 0, -20, -40}, {-30, 0, 10, 15, 15, 10, 0, -30},
     {-30, 5, 15, 20, 20, 15, 5, -30}, {-30, 0, 15, 20, 20, 15, 0, -30}, {-30, 5, 10, 15, 15, 10, 5, -30},
     {-40, -20, 0, 0, 0, 0, -20, -40}, {-50, -40, -30, -30, -30, -30, -40, -50}},
    // Bishop
    {{-20, -10, -10, -10, -10, -10, -10, -20}, {-10, 0, 0, 0, 0, 0, 0, -10}, {-10, 0, 10, 10, 10, 10, 0, -10},
     {-10, 5, 5, 10, 10, 5, 5, -10}, {-10, 0, 10, 10, 10, 10, 0, -10}, {-10, 10, 10, 10, 10, 10, 10, -10},
     {-10, 5, 0, 0, 0, 0, 5, -10}, {-20, -10, -10, -10, -10, -10, -10, -20}},
    // Rook
    {{0, 0, 0, 5, 5, 0, 0, 0}, {-5, 0, 0, 0, 0, 0, 0, -5}, {-5, 0, 0, 0, 0, 0, 0, -5},
     {-5, 0, 0, 0, 0, 0, 0, -5}, {-5, 0, 0, 0, 0, 0, 0, -5}, {-5, 0, 0, 0, 0, 0, 0, -5},
     {5, 10, 10, 10, 10, 10, 10, 5}, {0, 0, 0, 0, 0, 0, 0, 0}},
    // Queen
    {{-20, -10, -10, -5, -5, -10, -10, -20}, {-10, 0, 0, 0, 0, 0, 0, -10}, {-10, 0, 5, 5, 5, 5, 0, -10},
     {-5, 0, 5, 5, 5, 5, 0, -5}, {0, 0, 5, 5, 5, 5, 0, -5}, {-10, 5, 5, 5, 5, 5, 0, -10},
     {-10, 0, 5, 0, 0, 0, 0, -10}, {-20, -10, -10, -5, -5, -10, -10, -20}},
    // King (middlegame)
    {{-30, -40, -40, -50, -50, -40, -40, -30}, {-30, -40, -40, -50, -50, -40, -40, -30},
     {-30, -40, -40, -50, -50, -40, -40, -30}, {-30, -40, -40, -50, -50, -40, -40, -30},
     {-20, -30, -30, -40, -40, -30, -30, -20}, {-10, -20, -20, -20, -20, -20, -20, -10},
     {20, 20, 0, 0, 0, 0, 20, 20}, {20, 30, 10, 0, 0, 10, 30, 20}}
};

// Global variables
inline VALUE VALUES[6];
inline LEVEL gdepth;
inline LEVEL glevel;
inline Move best_move;
inline NODES nodes;
inline int newpv;
inline int pvsready;
inline s4 gmode;
inline s4 stm;

// Elapsed time management
class Elapsed {
public:
    int seconds = 0;
    int useconds = 0;
    clock_t clock0 = clock();
    clock_t clock1 = clock();
    int diffclock = 0;

    void init() {
        seconds = useconds = diffclock = 0;
        clock0 = clock1 = clock();
    }

    void update() {
        clock1 = clock();
        diffclock = clock1 - clock0;
        if (diffclock < 0) diffclock = 0;
        if (diffclock > 655360000) diffclock = 0;
        useconds += diffclock;
        seconds += useconds / 1000000;
        useconds %= 1000000;
        clock0 = clock();
    }

    [[nodiscard]] double dclock() const {
        return static_cast<double>(seconds) + static_cast<double>(useconds) / 1000000.0;
    }
};

// Chess tree node for search
class Tree {
public:
    Board curr_board;
    Board next_board;
    LEVEL bl_len = 0;
    LEVEL depth = 0;
    LEVEL level = 0;
    std::array<Move, MAXLEVEL> best_line;
    Move curr_move;
    MOVEINDEX curr_index = 0;
    MOVEINDEX max_index = 0;
    MoveList legal_moves;
    VALUE alpha = ALPHA;
    VALUE best = 0;
    VALUE beta = BETA;
    VALUE value = 0;
    std::array<VALUE, MAXINDEX> valuelist;
};

// Board management class
class ChessBoard {
private:
    static Board initial_board() {
        return {{
            {WR, WN, WB, WQ, WK, WB, WN, WR},
            {WP, WP, WP, WP, WP, WP, WP, WP},
            {UO, UO, UO, UO, UO, UO, UO, UO},
            {UO, UO, UO, UO, UO, UO, UO, UO},
            {UO, UO, UO, UO, UO, UO, UO, UO},
            {UO, UO, UO, UO, UO, UO, UO, UO},
            {BP, BP, BP, BP, BP, BP, BP, BP},
            {BR, BN, BB, BQ, BK, BB, BN, BR},
            {1, 1, 1, 1, 0, 0, 0, 0}
        }};
    }

public:
    static void copy_board(const Board& src, Board& dest) {
        dest = src;
    }

    static int board_cmp(const Board& src, const Board& dest) {
        for (u5 y = 0; y < 9; ++y)
            for (u5 x = 0; x < 8; ++x)
                if (dest[y][x] != src[y][x])
                    return 1;
        return 0;
    }

    static void transpose(Board& board) {
        for (u5 y = 0; y < 4; ++y)
            for (u5 x = 0; x < 8; ++x)
                std::swap(board[y][x], board[7 - y][x]);
        for (u5 y = 0; y < 8; ++y)
            for (u5 x = 0; x < 8; ++x)
                board[y][x] = -board[y][x];
        std::swap(board[8][0], board[8][2]);
        std::swap(board[8][1], board[8][3]);
    }

    static void load(Board& board) {
        std::ifstream f(BRDFILE);
        if (!f) {
            warn("Cannot open .brd file for read");
            return;
        }
        s5 pp;
        for (u5 y = 8; y > 0; --y)
            for (u5 x = 0; x < 8; ++x) {
                f >> pp;
                board[y - 1][x] = pp;
            }
        for (u5 x = 0; x < 8; ++x)
            board[8][x] = (x < 4);
        f >> stm;
        f.close();
        show_board(board, stdout);
        if (stm)
            transpose(board);
    }

    static void save(const Board& board) {
        std::ofstream f(BRDFILE);
        if (!f) {
            warn("Cannot open .brd file for write");
            return;
        }
        for (s5 y = 8; y > 0; --y) {
            for (s5 x = 0; x < 8; ++x)
                f << board[y - 1][x] << " ";
            f << "\n";
        }
        f << stm << "\n";
        f.flush();
        f.close();
        if (stm)
            transpose(const_cast<Board&>(board));
    }

    static void parse_fen(Board& board) {
        std::ifstream f(FENFILE);
        if (!f) {
            warn("Cannot open start.fen file");
            return;
        }
        int x = 1, y = 8, ch;
        s5 pp;
        while ((ch = f.get()) != EOF && ch != ' ') {
            pp = 0;
            if (ch >= '1' && ch <= '8') {
                while (ch > '0') {
                    board[y - 1][x - 1] = 0;
                    ++x;
                    --ch;
                }
                if (x == 9) {
                    x = 1;
                    --y;
                    if (!y) {
                        ch = f.get();
                        break;
                    }
                }
                continue;
            } else if (ch != '/') {
                switch (ch) {
                    case 'p': pp = BP; break;
                    case 'n': pp = BN; break;
                    case 'b': pp = BB; break;
                    case 'r': pp = BR; break;
                    case 'q': pp = BQ; break;
                    case 'k': pp = BK; break;
                    case 'P': pp = WP; break;
                    case 'N': pp = WN; break;
                    case 'B': pp = WB; break;
                    case 'R': pp = WR; break;
                    case 'Q': pp = WQ; break;
                    case 'K': pp = WK; break;
                    default: pp = BS;
                }
                if (pp > BS)
                    board[y - 1][x - 1] = pp;
                ++x;
                if (x == 9) {
                    x = 1;
                    --y;
                    if (!y) {
                        ch = f.get();
                        break;
                    }
                }
            }
        }
        ch = f.get();
        f.close();
        stm = (ch == 'w') ? 0 : (ch == 'b') ? 1 : (warn("Cannot set stm variable"), 0);
    }

    static void show_board(const Board& board, std::FILE* f) {
        for (u5 y = 0; y < 8; ++y) {
            fprintf(f, "%d", 8 - y);
            for (u5 x = 0; x < 8; ++x) {
                s3 t = board[7 - y][x];
                switch (t) {
                    case UO: fprintf(f, " . "); break;
                    case WP: fprintf(f, " P "); break;
                    case WN: fprintf(f, " N "); break;
                    case WB: fprintf(f, " B "); break;
                    case WR: fprintf(f, " R "); break;
                    case WQ: fprintf(f, " Q "); break;
                    case WK: fprintf(f, " K "); break;
                    case BP: fprintf(f, " p "); break;
                    case BN: fprintf(f, " n "); break;
                    case BB: fprintf(f, " b "); break;
                    case BR: fprintf(f, " r "); break;
                    case BQ: fprintf(f, " q "); break;
                    case BK: fprintf(f, " k "); break;
                    default: warn("Wrong piece code");
                }
            }
            fprintf(f, "\n");
        }
        fprintf(f, "  a  b  c  d  e  f  g  h\n\n");
        fflush(f);
    }

    static Board get_init() {
        return initial_board();
    }
};

// Move generation and manipulation
class MoveGenerator {
public:
    static void copy_move(const Move& src, Move& dest) {
        dest = src;
    }

    static s4 move_cmp(const Move& src, const Move& dest) {
        for (u5 u = 0; u < 4; ++u)
            if (src[u] != dest[u])
                return u + 1;
        return 0;
    }

    static void addm(s5 y, s5 x, s5 y1, s5 x1, MOVEINDEX& curr_index, MoveList& movelist) {
        movelist[curr_index] = {y, x, y1, x1, 0, 0};
        ++curr_index;
        if (curr_index >= MAXINDEX)
            warn("Index too big");
    }

    static void addprom(s5 y, s5 x, s5 y1, s5 x1, s5 to, MOVEINDEX& curr_index, MoveList& movelist) {
        movelist[curr_index] = {y, x, y1, x1, to, 0};
        ++curr_index;
        if (curr_index >= MAXINDEX)
            warn("Index too big");
    }

    static void castle(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist) {
        if (y != 0 || x != 4 || board[y][x] != WK)
            return;
        Board aux;
        ChessBoard::copy_board(board, aux);
        if (board[0][0] == WR && board[0][1] == 0 && board[0][2] == 0 && board[0][3] == 0 && board[8][0] == 1) {
            aux[0][2] = WK;
            aux[0][3] = WK;
            if (!in_check(aux))
                addm(0, 4, 0, 2, curr_index, movelist);
        }
        if (board[0][7] == WR && board[0][6] == 0 && board[0][5] == 0 && board[8][1] == 1) {
            aux[0][5] = WK;
            aux[0][6] = WK;
            if (!in_check(aux))
                addm(0, 4, 0, 6, curr_index, movelist);
        }
    }

    static void nonslider(const Board& board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX& curr_index, MoveList& movelist) {
        if (y + dy < 0 || y + dy > 7 || x + dx < 0 || x + dx > 7)
            return;
        if (board[y + dy][x + dx] <= 0)
            addm(y, x, y + dy, x + dx, curr_index, movelist);
    }

    static void slider(const Board& board, s5 y, s5 x, s3 dy, s3 dx, MOVEINDEX& curr_index, MoveList& movelist) {
        s5 y1 = y, x1 = x;
        while (true) {
            y1 += dy;
            x1 += dx;
            if (y1 < 0 || y1 > 7 || x1 < 0 || x1 > 7)
                return;
            if (board[y1][x1] < 0) {
                addm(y, x, y1, x1, curr_index, movelist);
                return;
            }
            if (board[y1][x1] > 0)
                return;
            addm(y, x, y1, x1, curr_index, movelist);
        }
    }

    static void genP(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist) {
        if (board[y + 1][x] == 0) {
            if (y != 6)
                addm(y, x, y + 1, x, curr_index, movelist);
            else
                for (s5 to = 5; to >= 2; --to)
                    addprom(y, x, y + 1, x, to, curr_index, movelist);
            if (y == 1 && board[y + 2][x] == 0)
                addm(y, x, y + 2, x, curr_index, movelist);
        }
        if (x > 0 && board[y + 1][x - 1] < 0) {
            if (y != 6)
                addm(y, x, y + 1, x - 1, curr_index, movelist);
            else
                for (s5 to = 5; to >= 2; --to)
                    addprom(y, x, y + 1, x - 1, to, curr_index, movelist);
        }
        if (x < 7 && board[y + 1][x + 1] < 0) {
            if (y != 6)
                addm(y, x, y + 1, x + 1, curr_index, movelist);
            else
                for (s5 to = 5; to >= 2; --to)
                    addprom(y, x, y + 1, x + 1, to, curr_index, movelist);
        }
    }

    static void genN(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist) {
        nonslider(board, y, x, +2, -1, curr_index, movelist);
        nonslider(board, y, x, +2, +1, curr_index, movelist);
        nonslider(board, y, x, +1, +2, curr_index, movelist);
        nonslider(board, y, x, +1, -2, curr_index, movelist);
        nonslider(board, y, x, -1, +2, curr_index, movelist);
        nonslider(board, y, x, -1, -2, curr_index, movelist);
        nonslider(board, y, x, -2, -1, curr_index, movelist);
        nonslider(board, y, x, -2, +1, curr_index, movelist);
    }

    static void genB(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist) {
        slider(board, y, x, +1, -1, curr_index, movelist);
        slider(board, y, x, +1, +1, curr_index, movelist);
        slider(board, y, x, -1, +1, curr_index, movelist);
        slider(board, y, x, -1, -1, curr_index, movelist);
    }

    static void genR(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist) {
        slider(board, y, x, +1, 0, curr_index, movelist);
        slider(board, y, x, 0, -1, curr_index, movelist);
        slider(board, y, x, 0, +1, curr_index, movelist);
        slider(board, y, x, -1, 0, curr_index, movelist);
    }

    static void genQ(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist) {
        genR(board, y, x, curr_index, movelist);
        genB(board, y, x, curr_index, movelist);
    }

    static void genK(const Board& board, s5 y, s5 x, MOVEINDEX& curr_index, MoveList& movelist, LEVEL depth) {
        nonslider(board, y, x, -1, -1, curr_index, movelist);
        nonslider(board, y, x, -1, 0, curr_index, movelist);
        nonslider(board, y, x, -1, +1, curr_index, movelist);
        nonslider(board, y, x, 0, -1, curr_index, movelist);
        nonslider(board, y, x, 0, +1, curr_index, movelist);
        nonslider(board, y, x, +1, -1, curr_index, movelist);
        nonslider(board, y, x, +1, 0, curr_index, movelist);
        nonslider(board, y, x, +1, +1, curr_index, movelist);
        if (depth == 1 && ALLOW_CASTLE)
            castle(board, y, x, curr_index, movelist);
    }

    static MOVEINDEX gendeep(const Board& board, MoveList& movelist, LEVEL depth) {
        MOVEINDEX curr_index = 0;
        for (s5 y = 7; y >= 0; --y)
            for (s5 x = 0; x < 8; ++x)
                switch (board[y][x]) {
                    case WP: genP(board, y, x, curr_index, movelist); break;
                    case WN: genN(board, y, x, curr_index, movelist); break;
                    case WB: genB(board, y, x, curr_index, movelist); break;
                    case WR: genR(board, y, x, curr_index, movelist); break;
                    case WQ: genQ(board, y, x, curr_index, movelist); break;
                    case WK: genK(board, y, x, curr_index, movelist, depth); break;
                    default: break;
                }
        return curr_index;
    }

    static MOVEINDEX gen(const Board& board, MoveList& movelist, LEVEL depth, const std::vector<Tree>& treea) {
        MOVEINDEX max_index = gendeep(board, movelist, 1);
        if (pvsready && depth && !newpv && glevel < treea[0].bl_len) {
            for (LEVEL level = 0; level < glevel; ++level)
                if (treea[level].curr_index) {
                    printf("Skip level %d\n", level);
                    fflush(stdout);
                    return max_index;
                }
            Move move = treea[0].best_line[glevel];
            for (MOVEINDEX curr_index = 0; curr_index < max_index; ++curr_index)
                if (!move_cmp(move, movelist[curr_index])) {
                    std::swap(movelist[0], movelist[curr_index]);
                    movelist[0] = move;
                    break;
                }
            return max_index;
        }
        if (!depth)
            return max_index;

        if (glevel < gdepth - S_DEPTH - 1) {
            std::vector<Tree> treeb(MAXLEVEL);
            std::array<VALUE, MAXINDEX> valuelist;
            for (MOVEINDEX curr_index = 0; curr_index < max_index; ++curr_index) {
                Board aux;
                Move move = movelist[curr_index];
                makemove(board, move, aux);
                ChessBoard::copy_board(aux, treeb[0].curr_board);
                treeb[0].level = 0;
                treeb[0].depth = S_DEPTH;
                treeb[0].alpha = ALPHA_DFL;
                treeb[0].beta = BETA_DFL;
                valuelist[curr_index] = -search(treeb, 0, 0, treea, treeb);
            }
            for (MOVEINDEX curr_index = 0; curr_index < max_index; ++curr_index)
                for (MOVEINDEX ncurr_index = curr_index + 1; ncurr_index < max_index; ++ncurr_index)
                    if (valuelist[ncurr_index] > valuelist[curr_index]) {
                        std::swap(movelist[ncurr_index], movelist[curr_index]);
                        std::swap(valuelist[ncurr_index], valuelist[curr_index]);
                    }
            MOVEINDEX newmax_index = glevel ? 4 : max_index;
            return std::min(max_index, newmax_index);
        }
        return max_index;
    }

    static void makemove(const Board& src, const Move& move, Board& dest) {
        ChessBoard::copy_board(src, dest);
        if (dest[move[0]][move[1]] == WK) {
            if (move[0] == 0 && move[2] == 0 && move[1] == 4) {
                if (move[3] == 2) {
                    dest[0][0] = 0;
                    dest[0][3] = WR;
                } else if (move[3] == 6) {
                    dest[0][7] = 0;
                    dest[0][5] = WR;
                }
            }
            dest[8][0] = dest[8][1] = 0;
        }
        if (dest[move[0]][move[1]] == WR && move[0] == 0) {
            if (move[1] == 0) dest[8][0] = 0;
            if (move[1] == 7) dest[8][1] = 0;
        }
        if (dest[move[0]][move[1]] == WP && move[0] == 6) {
            dest[move[2]][move[3]] = move[4] ? move[4] : WQ;
            dest[move[0]][move[1]] = 0;
            ChessBoard::transpose(dest);
            return;
        }
        if (dest[move[0]][move[1]] == WP && move[0] == 4 && move[1] != move[3] && dest[move[0]][move[3]] == BP)
            dest[move[0]][move[3]] = 0;
        dest[move[2]][move[3]] = dest[move[0]][move[1]];
        dest[move[0]][move[1]] = 0;
        ChessBoard::transpose(dest);
    }

    static void show_move(const Move& move, const Board& board, u5 stm, char* buf) {
        Board aux;
        char* p = buf;
        if (board[move[0]][move[1]] == WK && move[1] == 4 && (move[3] == 6 || move[3] == 2)) {
            p += sprintf(p, move[3] == 6 ? "O-O" : "O-O-O");
            return;
        }
        switch (board[move[0]][move[1]]) {
            case WN: p += sprintf(p, "N"); break;
            case WB: p += sprintf(p, "B"); break;
            case WR: p += sprintf(p, "R"); break;
            case WQ: p += sprintf(p, "Q"); break;
            case WK: p += sprintf(p, "K"); break;
            default: break;
        }
        p += sprintf(p, "%c", 97 + move[1]);
        p += sprintf(p, stm ? "%d" : "%d", stm ? 9 - (move[0] + 1) : move[0] + 1);
        if ((board[move[0]][move[1]] == WP && move[1] != move[3]) || (board[move[0]][move[1]] > 1 && board[move[2]][move[3]] < 0))
            p += sprintf(p, "x");
        p += sprintf(p, "%c", 97 + move[3]);
        p += sprintf(p, stm ? "%d" : "%d", stm ? 9 - (move[2] + 1) : move[2] + 1);
        makemove(board, move, aux);
        if (in_check(aux))
            p += sprintf(p, "+");
        *p = 0;
    }

    static s4 in_check(const Board& board) {
        Board aux;
        MoveList movelist;
        ChessBoard::copy_board(board, aux);
        ChessBoard::transpose(aux);
        MOVEINDEX max_index = gendeep(aux, movelist, 0);
        for (MOVEINDEX curr_index = 0; curr_index < max_index; ++curr_index)
            if (aux[movelist[curr_index][2]][movelist[curr_index][3]] == BK)
                return 1;
        return 0;
    }
};

// Search and evaluation
class Search {
public:
    static VALUE eval(const Board& board, LEVEL level, const std::vector<Tree>& treea) {
        Board aux;
        int ivalue = 0, kings = 0;
        VALUE pvalue = 0;
        ++nodes;
        if ((nodes % SKIPFRAMES) == 0) {
            Elapsed elapsed;
            elapsed.update();
        }
        for (u5 y = 0; y < 8; ++y)
            for (u5 x = 0; x < 8; ++x) {
                if (board[y][x] > 0)
                    ivalue += pcsq[board[y][x] - 1][y][x];
                if (board[y][x] < 0)
                    ivalue -= pcsq[-board[y][x] - 1][7 - y][x];
                switch (board[y][x]) {
                    case WP:
                        switch (y) {
                            case 1: case 2: case 3: case 4: ivalue += 100; break;
                            case 5: ivalue += 200; break;
                            case 6: ivalue += 400; break;
                            default: break;
                        }
                        break;
                    case WN: case WB: case WR: case WQ:
                        ivalue += VALUES[static_cast<u5>(board[y][x])];
                        break;
                    case BP:
                        switch (y) {
                            case 6: case 5: case 4: case 3: ivalue -= 100; break;
                            case 2: ivalue -= 200; break;
                            case 1: ivalue -= 400; break;
                            default: break;
                        }
                        break;
                    case BN: case BB: case BR: case BQ:
                        ivalue -= VALUES[static_cast<u5>(-board[y][x])];
                        break;
                    case WK: ++kings; break;
                    case BK: --kings; break;
                    default: break;
                }
            }
        for (u5 y = 0; y < 8; ++y)
            for (u5 x = 0; x < 8; ++x) {
                u5 x1 = x > 3 ? 7 - x : x;
                u5 y1 = y > 3 ? 7 - y : y;
                if (board[y][x] < 0)
                    pvalue -= (1 + std::min(x1, y1));
                else if (board[y][x] > 0)
                    pvalue += (1 + std::min(x1, y1));
            }
        if (kings)
            return kings > 0 ? MAXVALUE - level : -MAXVALUE + level;
        VALUE value = ivalue + pvalue;
        if (treea[level].depth == 1) {
            ChessBoard::copy_board(board, aux);
            ChessBoard::transpose(aux);
            if (MoveGenerator::in_check(aux))
                return MAXVALUE - (level + 1);
        }
        if (treea[level].depth / 2 == 1) {
            if (MoveGenerator::in_check(board))
                return -2000 + value + level;
            if (value > treea[level].alpha) {
                value *= 10;
                if (value > 1500)
                    value = 2000 - level;
                return value;
            }
        }
        return value + (rand() % 19 - 9) + (level > 1 ? treea[level - 2].max_index - treea[level - 1].max_index : 0);
    }

    static VALUE search(std::vector<Tree>& tree_, LEVEL level, LEVEL depth, const std::vector<Tree>& treea, std::vector<Tree>& treeb) {
        Tree& tree = tree_[level];
        VALUE value = eval(tree.curr_board, level, treea);
        if (newpv)
            tree.bl_len = 0;
        if (value < -THRESHOLD)
            return value;
        if (tree.depth == 0 || (tree.depth <= OVERDEPTH && value > -(PAWNUNIT >> 1)))
            return value;
        if (depth)
            glevel = level;
        tree.max_index = MoveGenerator::gen(tree.curr_board, tree.legal_moves, depth, treea);
        if (tree.max_index == 0)
            return -MAXVALUE + level;
        if (newpv)
            tree.bl_len = 1;
        tree.best = -MAXVALUE;
        for (tree.curr_index = 0; tree.curr_index < tree.max_index; ++tree.curr_index) {
            Tree& ntree = tree_[level + 1];
            MoveGenerator::copy_move(tree.legal_moves[tree.curr_index], tree.curr_move);
            MoveGenerator::makemove(tree.curr_board, tree.curr_move, tree.next_board);
            ChessBoard::copy_board(tree.next_board, ntree.curr_board);
            if (depth && level < 1 && tree.curr_index && pvsready) {
                ntree.level = tree.level + 1;
                ntree.depth = tree.depth - 1;
                ntree.alpha = -tree.alpha - 1;
                ntree.beta = -tree.alpha;
                tree.value = -search(tree_, level + 1, depth, treea, treeb);
                if (tree.value <= tree.alpha)
                    continue;
            }
            ntree.level = tree.level + 1;
            ntree.depth = tree.depth - 1;
            ntree.alpha = -tree.beta;
            ntree.beta = -tree.alpha;
            tree.value = -search(tree_, level + 1, depth, treea, treeb);
            if (!newpv)
                ntree.bl_len = 0;
            newpv = 1;
            if (tree.value > tree.best) {
                tree.best = tree.value;
                tree.bl_len = ntree.bl_len + 1;
                MoveGenerator::copy_move(tree.curr_move, tree.best_line[0]);
                for (LEVEL bl_lev = 0; bl_lev < ntree.bl_len; ++bl_lev)
                    MoveGenerator::copy_move(ntree.best_line[bl_lev], tree.best_line[bl_lev + 1]);
                if (level == 0 && depth == 1 && gmode == 4) {
                    Elapsed elapsed;
                    elapsed.update();
                    double delapsed = elapsed.dclock();
                    Board aux, aux2;
                    ChessBoard::copy_board(treea[0].curr_board, aux);
                    printf("Depth: %u*\nEvaluation: %.2f\nBranching factor: %.2f\nBest variation: ",
                           treea[0].depth - OVERDEPTH, static_cast<double>(treea[0].best) / PAWNUNIT,
                           std::pow(static_cast<double>(nodes), 1.0 / (treea[0].depth - OVERDEPTH)));
                    char buf[80];
                    for (LEVEL i = 0; i < treea[0].bl_len; ++i) {
                        MoveGenerator::show_move(treea[0].best_line[i], aux, (i + stm) % 2, buf);
                        MoveGenerator::makemove(aux, treea[0].best_line[i], aux2);
                        ChessBoard::copy_board(aux2, aux);
                        printf("%s ", buf);
                    }
                    printf("\n");
                    if (treea[0].bl_len & 1)
                        ChessBoard::transpose(aux);
                    printf("Elapsed: %.2f\nNPS: %u\n\n", delapsed, static_cast<unsigned>(static_cast<double>(nodes) / delapsed));
                    fflush(stdout);
                }
                if (tree.best > tree.alpha)
                    tree.alpha = tree.best;
                if (tree.alpha >= tree.beta)
                    return tree.beta;
            }
        }
        return tree.best;
    }
};

// Analysis class
class Analysis {
public:
    static void run() {
        Board start, aux, aux2;
        char buf[80];
        std::vector<Tree> treea(MAXLEVEL), treeb(MAXLEVEL);
        Elapsed elapsed;

#if NOEDIT == 3
        ChessBoard::parse_fen(start);
        exit(0);
#elif NOEDIT == 2
        ChessBoard::parse_fen(start);
        ChessBoard::save(start);
#elif NOEDIT == 1
        parse_pgn();
        ChessBoard::load(start);
#else
        ChessBoard::load(start);
#endif
        ChessBoard::show_board(start, stdout);
        elapsed.init();
        nodes = 0;
        pvsready = 0;
        for (LEVEL depth = S_DEPTH + 1; depth < MAXLEVEL; ++depth) {
            Tree& tree = treea[0];
            ChessBoard::copy_board(start, tree.curr_board);
            tree.level = 0;
            tree.depth = depth + OVERDEPTH;
            gdepth = tree.depth;
            tree.alpha = ALPHA;
            tree.beta = BETA;
            newpv = 0;
            tree.best = Search::search(treea, 0, 1, treea, treeb);
            pvsready = 1;
            elapsed.update();
            double delapsed = elapsed.dclock();
            ChessBoard::copy_board(start, aux);
            printf("Depth: %u\nEvaluation: %.2f\nBranching factor: %.2f\nBest variation: ",
                   depth, static_cast<double>(tree.best) / PAWNUNIT, std::pow(static_cast<double>(nodes), 1.0 / depth));
            for (LEVEL i = 0; i < tree.bl_len; ++i) {
                MoveGenerator::show_move(tree.best_line[i], aux, (i + stm) % 2, buf);
                MoveGenerator::makemove(aux, tree.best_line[i], aux2);
                ChessBoard::copy_board(aux2, aux);
                printf("%s ", buf);
            }
            printf("\n");
            if (tree.bl_len & 1)
                ChessBoard::transpose(aux);
            printf("Elapsed: %.2f\nNPS: %u\n\n", delapsed, static_cast<unsigned>(static_cast<double>(nodes) / delapsed));
            fflush(stdout);
        }
    }
};

// Utility functions
inline void warn(const char* msg) {
    fprintf(stderr, "\nwarn %s\n", msg);
}

void load_values() {
    std::ifstream vf("start.bpf");
    if (!vf)
        exit(1);
    for (int i = 1; i < 6; ++i) {
        int j;
        vf >> j >> VALUES[i];
        if (j != i)
            exit(0);
    }
    VALUES[0] = 0;
    vf.close();
}

void parse_pgn() {
    if (system("pgn-extract -F start.pgn > pf") != 0) {
        fprintf(stderr, "Error running pgn-extract\n");
        return;
    }
    std::ifstream f("pf");
    std::ofstream g(FENFILE);
    if (!f || !g) {
        fprintf(stderr, "Error opening files\n");
        if (f) f.close();
        if (g) g.close();
        return;
    }
    int ch;
    Board board = ChessBoard::get_init();
    while ((ch = f.get()) != '{') {}
    while ((ch = f.get()) != '"') {}
    while ((ch = f.get()) != '"') g.put(ch);
    f.close();
    g.close();
    ChessBoard::parse_fen(board);
    ChessBoard::save(board);
}

void setup_board(Board& board) {
    s5 x = 3, y = 3;
    unsigned int symbol = 0;
    ChessBoard::parse_fen(board);
    while (true) {
        printf("\033[2J\033[1;1H");
        ChessBoard::show_board(board, stdout);
        system("cat start.fen");
        printf("enter square %d %c%d\n", stm, x + 97, y + 1);
        fflush(stdout);
        scanf("%u", &symbol);
        fflush(stdin);
        switch (symbol) {
            case 4: return;
            case 20: board[y][x] = 0; break;
            case 31: case 32: case 33: case 34: case 35: case 36:
                board[y][x] = symbol - 30; break;
            case 41: case 42: case 43: case 44: case 45: case 46:
                board[y][x] = 40 - symbol; break;
            case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58:
                x = symbol - 51; break;
            case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68:
                y = symbol - 61; break;
            case 70: ChessBoard::copy_board(ChessBoard::get_init(), board); break;
            case 71: ChessBoard::load(board); break;
            case 72: ChessBoard::save(board); break;
            case 73: stm = 1 - stm; board[8][4] = stm; break;
            case 74: exit(0); break;
            case 75: ChessBoard::transpose(board); break;
            case 76: ChessBoard::parse_fen(board); break;
            default: break;
        }
    }
}

int main(int argc, char* argv[]) {
    gmode = 4;
    load_values();
    Analysis::run();
    return 0;
}