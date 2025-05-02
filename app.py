from flask import Flask, request, render_template_string
import subprocess
import os

app = Flask(__name__)

# HTML template for the UI
HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>Baeagn Chess Engine</title>
</head>
<body>
    <h1>Analyze PGN with Baeagn</h1>
    <form method="POST">
        <label for="depth">Depth:</label>
        <input type="number" name="depth" value="3" min="1" required><br><br>
        <label for="pgn">PGN (or leave empty to use ./start.pgn):</label><br>
        <textarea name="pgn" rows="10" cols="50"></textarea><br>
        <button type="submit">Analyze</button>
    </form>
    {% if result %}
    <h2>Result:</h2>
    <pre>{{ result }}</pre>
    {% endif %}
</body>
</html>
"""

@app.route('/', methods=['GET', 'POST'])
def analyze():
    result = None
    if request.method == 'POST':
        depth = request.form['depth']
        pgn = request.form['pgn'].strip()
        
        # Write PGN to ./start.pgn if provided
        if pgn:
            with open('start.pgn', 'w') as f:
                f.write(pgn)
        
        # Run Baeagn engine
        cmd = ['./baeagn', depth]
        process = subprocess.run(cmd, capture_output=True, text=True)
        result = process.stdout
    
    return render_template_string(HTML, result=result)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)