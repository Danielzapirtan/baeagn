import subprocess
import time
import os
import sys
import chess
import chess.pgn
from io import StringIO

# Ensure the start.fen file exists
if not os.path.exists('start.fen'):
    with open('start.fen', 'w') as f:
        f.write('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')

def get_current_fen():
    with open('start.fen', 'r') as f:
        return f.read().strip()

def save_fen(fen):
    with open('start.fen', 'w') as f:
        f.write(fen)

def pgn_to_fen(pgn_text):
    try:
        game = chess.pgn.read_game(StringIO(pgn_text))
        if game is None:
            return None
        board = game.board()
        for move in game.mainline_moves():
            board.push(move)
        return board.fen()
    except Exception as e:
        print(f"Error converting PGN to FEN: {e}")
        return None

def analyze(pgn):
    depth = 64
    
    # If PGN is provided, convert it to FEN
    if pgn:
        converted_fen = pgn_to_fen(pgn)
        if converted_fen:
            fen = converted_fen
        else:
            return jsonify({'error': 'Invalid PGN'}), 400
    
    if not fen:
        fen = get_current_fen()
    else:
        save_fen(fen)
    
    # Validate FEN
    try:
        board = chess.Board(fen)
    except ValueError:
        return jsonify({'error': 'Invalid FEN'}), 400
    
    # Save the FEN to start.fen
    save_fen(fen)
    
    result = subprocess.run(
        ['./baeagn', str(depth)],
        input=fen,
        text=True
    )
        
if __name__ == '__main__':
    pgn = sys.argv[1]
    analyze(pgn)

