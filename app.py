from flask import Flask, render_template, request, jsonify
import subprocess
import time
import os
import chess
import chess.pgn
from io import StringIO

app = Flask(__name__)

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

@app.route('/')
def index():
    return render_template('index.html', fen=get_current_fen())

@app.route('/analyze', methods=['POST'])
def analyze():
    data = request.json
    depth = int(data.get('depth', 10))  # Default depth 10
    fen = data.get('fen', '')
    pgn = data.get('pgn', '')
    
    # Validate depth (6-15)
    depth = max(6, min(15, depth))
    
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
    
    # Start the timer
    start_time = time.time()
    
    try:
        # Call the engine with increased timeout (60 seconds)
        result = subprocess.run(
            ['./baeagn', str(depth)],
            input=fen,
            text=True,
            capture_output=True,
            timeout=60  # timeout after 60 seconds
        )
        
        elapsed_time = round(time.time() - start_time, 2)
        
        if result.returncode != 0:
            return jsonify({
                'error': f'Engine error: {result.stderr}',
                'time': elapsed_time,
                'stdout': result.stdout,
                'stderr': result.stderr
            }), 500
        
        # Return the complete stdout without parsing
        return jsonify({
            'fen': fen,
            'stdout': result.stdout,
            'stderr': result.stderr,
            'time': elapsed_time
        })
        
    except subprocess.TimeoutExpired as e:
        elapsed_time = round(time.time() - start_time, 2)
        return jsonify({
            'stdout': e.stdout.decode('utf-8') if e.stdout else '',
            'stderr': e.stderr.decode('utf-8') if e.stderr else '',
            'time': elapsed_time,
            'message': 'Analysis completed (time limit reached)'
        })
    except Exception as e:
        elapsed_time = round(time.time() - start_time, 2)
        return jsonify({
            'error': str(e),
            'time': elapsed_time
        }), 500

if __name__ == '__main__':
    app.run(debug=True)