#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Constants
constexpr int MAX_LEVEL = 253;
constexpr int MAX_INDEX = 200;
constexpr int FRAMES_PER_SEC = 32;
constexpr int NPS = 3 << 18;
constexpr int SKIP_FRAMES = NPS / FRAMES_PER_SEC;
constexpr int ALPHA_DEFAULT = -20000;
constexpr int BETA_DEFAULT = 20000;
constexpr int MAX_VALUE = 20000;
constexpr int PAWN_UNIT = 100;
constexpr int THRESHOLD = 15000;
constexpr int SEARCH_DEPTH = 4;
constexpr int OVER_DEPTH = 1;

// Piece codes
enum class Piece : int {
    EMPTY = 0,
    WHITE_PAWN = 1,
    WHITE_KNIGHT = 2,
    WHITE_BISHOP = 3,
    WHITE_ROOK = 4,
    WHITE_QUEEN = 5,
    WHITE_KING = 6,
    BLACK_PAWN = -1,
    BLACK_KNIGHT = -2,
    BLACK_BISHOP = -3,
    BLACK_ROOK = -4,
    BLACK_QUEEN = -5,
    BLACK_KING = -6,
    WHITE_M = 7,
    BLACK_M = -7,
    BS = -8
};

// Game modes
enum class GameMode {
    ANALYSIS,
    PLAY,
    SETUP
};

// Move representation
struct Move {
    int from_y, from_x, to_y, to_x, promotion{0};
    
    Move() = default;
    Move(int fy, int fx, int ty, int tx, int prom = 0) 
        : from_y(fy), from_x(fx), to_y(ty), to_x(tx), promotion(prom) {}
    
    bool operator==(const Move& other) const {
        return from_y == other.from_y && from_x == other.from_x &&
               to_y == other.to_y && to_x == other.to_x &&
               promotion == other.promotion;
    }
    
    bool operator!=(const Move& other) const {
        return !(*this == other);
    }
};

// Board representation
class ChessBoard {
private:
    std::array<std::array<Piece, 8>, 9> squares;
    
public:
    ChessBoard() { clear(); }
    
    void clear() {
        for (auto& row : squares) {
            row.fill(Piece::EMPTY);
        }
    }
    
    Piece& operator()(int y, int x) { return squares[y][x]; }
    const Piece& operator()(int y, int x) const { return squares[y][x]; }
    
    std::array<std::array<Piece, 8>, 9>& getSquares() { return squares; }
    const std::array<std::array<Piece, 8>, 9>& getSquares() const { return squares; }
    
    bool operator==(const ChessBoard& other) const {
        return squares == other.squares;
    }
    
    bool operator!=(const ChessBoard& other) const {
        return !(*this == other);
    }
};

// Timer for performance measurement
class Timer {
private:
    clock_t clock_start, clock_end;
    int seconds{0};
    int useconds{0};
    
public:
    void start() {
        clock_start = clock();
    }
    
    void update() {
        clock_end = clock();
        int diff = clock_end - clock_start;
        if (diff < 0) diff = 0;
        if (diff > 655360000) diff = 0;
        
        useconds += diff;
        seconds += useconds / 1000000;
        useconds %= 1000000;
        clock_start = clock();
    }
    
    double elapsed() const {
        return seconds + useconds / 1000000.0;
    }
};

// Evaluation component
class Evaluator {
private:
    static constexpr std::array<std::array<std::array<int, 8>, 8>, 6> PIECE_SQUARE_TABLES = {{
        // Pawn
        {{
            {0,   0,   0,   0,   0,   0,   0,   0},
            {10,  10,  10,  10,  10,  10,  10,  10},
            {25,  25,  25,  25,  25,  25,  25,  25},
            {35,  35,  35,  40,  40,  35,  35,  35},
            {45,  45,  45,  50,  50,  45,  45,  45},
            {60,  60,  60,  65,  65,  60,  60,  60},
            {80,  80,  80,  85,  85,  80,  80,  80},
            {0,   0,   0,   0,   0,   0,   0,   0}
        }},
        // Knight
        {{
            {-50, -40, -30, -30, -30, -30, -40, -50},
            {-40, -20,   0,   5,   5,   0, -20, -40},
            {-30,   0,  10,  15,  15,  10,   0, -30},
            {-30,   5,  15,  20,  20,  15,   5, -30},
            {-30,   0,  15,  20,  20,  15,   0, -30},
            {-30,   5,  10,  15,  15,  10,   5, -30},
            {-40, -20,   0,   0,   0,   0, -20, -40},
            {-50, -40, -30, -30, -30, -30, -40, -50}
        }},
        // Bishop
        {{
            {-20, -10, -10, -10, -10, -10, -10, -20},
            {-10,   0,   0,   0,   0,   0,   0, -10},
            {-10,   0,  10,  10,  10,  10,   0, -10},
            {-10,   5,   5,  10,  10,   5,   5, -10},
            {-10,   0,  10,  10,  10,  10,   0, -10},
            {-10,  10,  10,  10,  10,  10,  10, -10},
            {-10,   5,   0,   0,   0,   0,   5, -10},
            {-20, -10, -10, -10, -10, -10, -10, -20}
        }},
        // Rook
        {{
            {0,   0,   0,   5,   5,   0,   0,   0},
            {-5,   0,   0,   0,   0,   0,   0,  -5},
            {-5,   0,   0,   0,   0,   0,   0,  -5},
            {-5,   0,   0,   0,   0,   0,   0,  -5},
            {-5,   0,   0,   0,   0,   0,   0,  -5},
            {-5,   0,   0,   0,   0,   0,   0,  -5},
            {5,  10,  10,  10,  10,  10,  10,   5},
            {0,   0,   0,   0,   0,   0,   0,   0}
        }},
        // Queen
        {{
            {-20, -10, -10,  -5,  -5, -10, -10, -20},
            {-10,   0,   0,   0,   0,   0,   0, -10},
            {-10,   0,   5,   5,   5,   5,   0, -10},
            {-5,   0,   5,   5,   5,   5,   0,  -5},
            {0,   0,   5,   5,   5,   5,   0,  -5},
            {-10,   5,   5,   5,   5,   5,   0, -10},
            {-10,   0,   5,   0,   0,   0,   0, -10},
            {-20, -10, -10,  -5,  -5, -10, -10, -20}
        }},
        // King
        {{
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-20, -30, -30, -40, -40, -30, -30, -20},
            {-10, -20, -20, -20, -20, -20, -20, -10},
            {20,  20,   0,   0,   0,   0,  20,  20},
            {20,  30,  10,   0,   0,  10,  30,  20}
        }}
    }};
    
    std::array<int, 6> pieceValues{0, 100, 320, 330, 500, 900};
    std::mt19937 rng{std::random_device{}()};
    
public:
    int evaluate(const ChessBoard& board, int level, int depth, int alpha) {
        int materialScore = 0;
        int positionalScore = 0;
        int kingCount = 0;
        
        // Evaluate each square
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                Piece piece = board(y, x);
                if (piece == Piece::EMPTY) continue;
                
                int pieceType = static_cast<int>(piece);
                bool isWhite = pieceType > 0;
                int absType = std::abs(pieceType) - 1;
                
                // Material and piece-square tables
                if (isWhite) {
                    materialScore += pieceValues[absType];
                    positionalScore += PIECE_SQUARE_TABLES[absType][y][x];
                    
                    // Pawn structure bonus
                    if (piece == Piece::WHITE_PAWN) {
                        switch (y) {
                            case 1: case 2: case 3: case 4: materialScore += 100; break;
                            case 5: materialScore += 200; break;
                            case 6: materialScore += 400; break;
                        }
                    }
                } else {
                    materialScore -= pieceValues[absType];
                    positionalScore -= PIECE_SQUARE_TABLES[absType][7 - y][x];
                    
                    // Pawn structure bonus
                    if (piece == Piece::BLACK_PAWN) {
                        switch (y) {
                            case 6: case 5: case 4: case 3: materialScore -= 100; break;
                            case 2: materialScore -= 200; break;
                            case 1: materialScore -= 400; break;
                        }
                    }
                }
                
                // King count for checkmate detection
                if (piece == Piece::WHITE_KING) kingCount++;
                if (piece == Piece::BLACK_KING) kingCount--;
            }
        }
        
        // Check for checkmate
        if (kingCount > 0) return MAX_VALUE - level;
        if (kingCount < 0) return -MAX_VALUE + level;
        
        int totalScore = materialScore + positionalScore;
        
        // Add small random factor to avoid repetition
        std::uniform_int_distribution<int> dist(-9, 9);
        totalScore += dist(rng);
        
        return totalScore;
    }
};

// Move generator
class MoveGenerator {
private:
    std::vector<Move> moves;
    
    void addMove(int fromY, int fromX, int toY, int toX, int promotion = 0) {
        if (moves.size() < MAX_INDEX) {
            moves.emplace_back(fromY, fromX, toY, toX, promotion);
        }
    }
    
    void generatePawnMoves(const ChessBoard& board, int y, int x, bool isWhite) {
        int direction = isWhite ? 1 : -1;
        int startRank = isWhite ? 1 : 6;
        int promotionRank = isWhite ? 6 : 1;
        
        // Forward move
        if (board(y + direction, x) == Piece::EMPTY) {
            if (y != promotionRank) {
                addMove(y, x, y + direction, x);
            } else {
                // Promotions
                for (int piece : {2, 3, 4, 5}) { // N, B, R, Q
                    addMove(y, x, y + direction, x, piece);
                }
            }
            
            // Double move from starting position
            if (y == startRank && board(y + 2 * direction, x) == Piece::EMPTY) {
                addMove(y, x, y + 2 * direction, x);
            }
        }
        
        // Captures
        for (int dx : {-1, 1}) {
            if (x + dx >= 0 && x + dx < 8) {
                Piece target = board(y + direction, x + dx);
                if (target != Piece::EMPTY && 
                    ((isWhite && static_cast<int>(target) < 0) || 
                     (!isWhite && static_cast<int>(target) > 0))) {
                    if (y != promotionRank) {
                        addMove(y, x, y + direction, x + dx);
                    } else {
                        for (int piece : {2, 3, 4, 5}) {
                            addMove(y, x, y + direction, x + dx, piece);
                        }
                    }
                }
            }
        }
    }
    
    void generateKnightMoves(const ChessBoard& board, int y, int x, bool isWhite) {
        const std::array<std::pair<int, int>, 8> offsets = {
            {{2, -1}, {2, 1}, {1, 2}, {1, -2},
            {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}}
        };
        
        for (const auto& [dy, dx] : offsets) {
            int newY = y + dy;
            int newX = x + dx;
            if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                Piece target = board(newY, newX);
                if (target == Piece::EMPTY || 
                    (isWhite && static_cast<int>(target) < 0) ||
                    (!isWhite && static_cast<int>(target) > 0)) {
                    addMove(y, x, newY, newX);
                }
            }
        }
    }
    
    void generateSliderMoves(const ChessBoard& board, int y, int x, bool isWhite, 
                           const std::vector<std::pair<int, int>>& directions) {
        for (const auto& [dy, dx] : directions) {
            for (int distance = 1; distance < 8; ++distance) {
                int newY = y + dy * distance;
                int newX = x + dx * distance;
                if (newY < 0 || newY >= 8 || newX < 0 || newX >= 8) break;
                
                Piece target = board(newY, newX);
                if (target == Piece::EMPTY) {
                    addMove(y, x, newY, newX);
                } else {
                    if ((isWhite && static_cast<int>(target) < 0) ||
                        (!isWhite && static_cast<int>(target) > 0)) {
                        addMove(y, x, newY, newX);
                    }
                    break;
                }
            }
        }
    }
    
public:
    std::vector<Move> generateMoves(const ChessBoard& board, bool whiteToMove) {
        moves.clear();
        
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                Piece piece = board(y, x);
                if (piece == Piece::EMPTY) continue;
                
                bool isWhite = static_cast<int>(piece) > 0;
                if (isWhite != whiteToMove) continue;
                
                switch (piece) {
                    case Piece::WHITE_PAWN:
                    case Piece::BLACK_PAWN:
                        generatePawnMoves(board, y, x, isWhite);
                        break;
                        
                    case Piece::WHITE_KNIGHT:
                    case Piece::BLACK_KNIGHT:
                        generateKnightMoves(board, y, x, isWhite);
                        break;
                        
                    case Piece::WHITE_BISHOP:
                    case Piece::BLACK_BISHOP:
                        generateSliderMoves(board, y, x, isWhite, 
                                          {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}});
                        break;
                        
                    case Piece::WHITE_ROOK:
                    case Piece::BLACK_ROOK:
                        generateSliderMoves(board, y, x, isWhite,
                                          {{1, 0}, {-1, 0}, {0, 1}, {0, -1}});
                        break;
                        
                    case Piece::WHITE_QUEEN:
                    case Piece::BLACK_QUEEN:
                        generateSliderMoves(board, y, x, isWhite,
                                          {{1, -1}, {1, 1}, {-1, 1}, {-1, -1},
                                           {1, 0}, {-1, 0}, {0, 1}, {0, -1}});
                        break;
                        
                    case Piece::WHITE_KING:
                    case Piece::BLACK_KING:
                        for (int dy = -1; dy <= 1; ++dy) {
                            for (int dx = -1; dx <= 1; ++dx) {
                                if (dy == 0 && dx == 0) continue;
                                int newY = y + dy;
                                int newX = x + dx;
                                if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8) {
                                    Piece target = board(newY, newX);
                                    if (target == Piece::EMPTY || 
                                        (isWhite && static_cast<int>(target) < 0) ||
                                        (!isWhite && static_cast<int>(target) > 0)) {
                                        addMove(y, x, newY, newX);
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
        
        return moves;
    }
};

// Search tree node
class SearchNode {
public:
    ChessBoard currentBoard;
    ChessBoard nextBoard;
    Move currentMove;
    std::vector<Move> legalMoves;
    std::vector<Move> bestLine;
    int depth{0};
    int level{0};
    int alpha{ALPHA_DEFAULT};
    int beta{BETA_DEFAULT};
    int bestValue{-MAX_VALUE};
    int currentValue{0};
    
    SearchNode() = default;
};

// Principal Variation Search
class PVS {
private:
    std::vector<std::unique_ptr<SearchNode>> treeA;
    std::vector<std::unique_ptr<SearchNode>> treeB;
    MoveGenerator moveGen;
    Evaluator evaluator;
    Timer timer;
    uint64_t nodes{0};
    bool pvsReady{false};
    bool newPV{false};
    int globalDepth{0};
    int globalLevel{0};
    
    int search(std::vector<std::unique_ptr<SearchNode>>& tree, int level, int depth) {
        auto& node = tree[level];
        int value = evaluator.evaluate(node->currentBoard, level, node->depth, node->alpha);
        
        if (newPV) {
            node->bestLine.clear();
        }
        
        if (value < -THRESHOLD || node->depth == 0) {
            return value;
        }
        
        if (depth) {
            globalLevel = level;
        }
        
        node->legalMoves = moveGen.generateMoves(node->currentBoard, true); // Assuming white to move
        
        if (node->legalMoves.empty()) {
            return -MAX_VALUE + level;
        }
        
        if (newPV) {
            node->bestLine.push_back(node->legalMoves[0]);
        }
        
        node->bestValue = -MAX_VALUE;
        
        for (size_t i = 0; i < node->legalMoves.size(); ++i) {
            node->currentMove = node->legalMoves[i];
            auto& nextNode = tree[level + 1];
            
            // Make move
            nextNode->currentBoard = node->currentBoard;
            // TODO: Implement makeMove function
            
            nextNode->level = node->level + 1;
            nextNode->depth = node->depth - 1;
            nextNode->alpha = -node->beta;
            nextNode->beta = -node->alpha;
            
            node->currentValue = -search(tree, level + 1, depth);
            
            if (!newPV) {
                nextNode->bestLine.clear();
            }
            newPV = true;
            
            if (node->currentValue > node->bestValue) {
                node->bestValue = node->currentValue;
                node->bestLine = nextNode->bestLine;
                node->bestLine.insert(node->bestLine.begin(), node->currentMove);
                
                if (node->bestValue > node->alpha) {
                    node->alpha = node->bestValue;
                }
                if (node->alpha >= node->beta) {
                    return node->beta;
                }
            }
        }
        
        return node->bestValue;
    }
    
public:
    PVS() {
        // Initialize search trees
        for (int i = 0; i < MAX_LEVEL; ++i) {
            treeA.push_back(std::make_unique<SearchNode>());
            treeB.push_back(std::make_unique<SearchNode>());
        }
    }
    
    void analyze(const ChessBoard& startPosition) {
        timer.start();
        nodes = 0;
        pvsReady = false;
        
        for (int depth = SEARCH_DEPTH + 1; depth < MAX_LEVEL; ++depth) {
            auto& root = treeA[0];
            root->currentBoard = startPosition;
            root->level = 0;
            root->depth = depth + OVER_DEPTH;
            globalDepth = root->depth;
            root->alpha = ALPHA_DEFAULT;
            root->beta = BETA_DEFAULT;
            newPV = false;
            
            root->bestValue = search(treeA, 0, 1);
            pvsReady = true;
            
            timer.update();
            double elapsed = timer.elapsed();
            
            // Print results
            std::cout << "Depth: " << depth << "\n";
            std::cout << "Evaluation: " << std::fixed << std::setprecision(2) 
                      << (static_cast<double>(root->bestValue) / PAWN_UNIT) << "\n";
            std::cout << "Nodes: " << nodes << "\n";
            std::cout << "Best variation: ";
            
            ChessBoard tempBoard = startPosition;
            for (const auto& move : root->bestLine) {
                // TODO: Implement move display and execution
                std::cout << "[" << move.from_y << "," << move.from_x << "->" 
                          << move.to_y << "," << move.to_x << "] ";
            }
            std::cout << "\n";
            std::cout << "Elapsed: " << elapsed << "s\n";
            std::cout << "NPS: " << static_cast<unsigned int>(nodes / elapsed) << "\n\n";
        }
    }
};

// Main chess engine class
class ChessEngine {
private:
    PVS searchEngine;
    ChessBoard currentBoard;
    GameMode mode{GameMode::ANALYSIS};
    bool sideToMove{false}; // false = white, true = black
    
public:
    void initialize() {
        setupInitialPosition();
    }
    
    void setupInitialPosition() {
        currentBoard.clear();
        
        // Set up initial position (simplified)
        // White pieces
        currentBoard(0, 0) = Piece::WHITE_ROOK;
        currentBoard(0, 1) = Piece::WHITE_KNIGHT;
        currentBoard(0, 2) = Piece::WHITE_BISHOP;
        currentBoard(0, 3) = Piece::WHITE_QUEEN;
        currentBoard(0, 4) = Piece::WHITE_KING;
        currentBoard(0, 5) = Piece::WHITE_BISHOP;
        currentBoard(0, 6) = Piece::WHITE_KNIGHT;
        currentBoard(0, 7) = Piece::WHITE_ROOK;
        
        for (int x = 0; x < 8; ++x) {
            currentBoard(1, x) = Piece::WHITE_PAWN;
        }
        
        // Black pieces
        currentBoard(7, 0) = Piece::BLACK_ROOK;
        currentBoard(7, 1) = Piece::BLACK_KNIGHT;
        currentBoard(7, 2) = Piece::BLACK_BISHOP;
        currentBoard(7, 3) = Piece::BLACK_QUEEN;
        currentBoard(7, 4) = Piece::BLACK_KING;
        currentBoard(7, 5) = Piece::BLACK_BISHOP;
        currentBoard(7, 6) = Piece::BLACK_KNIGHT;
        currentBoard(7, 7) = Piece::BLACK_ROOK;
        
        for (int x = 0; x < 8; ++x) {
            currentBoard(6, x) = Piece::BLACK_PAWN;
        }
        
        sideToMove = false;
    }
    
    void displayBoard() const {
        std::cout << "\n  a b c d e f g h\n";
        for (int y = 7; y >= 0; --y) {
            std::cout << (y + 1) << " ";
            for (int x = 0; x < 8; ++x) {
                char symbol = '.';
                switch (currentBoard(y, x)) {
                    case Piece::WHITE_PAWN: symbol = 'P'; break;
                    case Piece::WHITE_KNIGHT: symbol = 'N'; break;
                    case Piece::WHITE_BISHOP: symbol = 'B'; break;
                    case Piece::WHITE_ROOK: symbol = 'R'; break;
                    case Piece::WHITE_QUEEN: symbol = 'Q'; break;
                    case Piece::WHITE_KING: symbol = 'K'; break;
                    case Piece::BLACK_PAWN: symbol = 'p'; break;
                    case Piece::BLACK_KNIGHT: symbol = 'n'; break;
                    case Piece::BLACK_BISHOP: symbol = 'b'; break;
                    case Piece::BLACK_ROOK: symbol = 'r'; break;
                    case Piece::BLACK_QUEEN: symbol = 'q'; break;
                    case Piece::BLACK_KING: symbol = 'k'; break;
                    default: symbol = '.';
                }
                std::cout << symbol << " ";
            }
            std::cout << (y + 1) << "\n";
        }
        std::cout << "  a b c d e f g h\n\n";
        std::cout << (sideToMove ? "Black" : "White") << " to move\n";
    }
    
    void runAnalysis() {
        std::cout << "Starting chess engine analysis...\n";
        displayBoard();
        searchEngine.analyze(currentBoard);
    }
};

int main() {
    try {
        ChessEngine engine;
        engine.initialize();
        engine.runAnalysis();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}