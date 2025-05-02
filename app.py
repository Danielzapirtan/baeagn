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
    <h1>Analyze FEN with Baeagn</h1>
    <form method="POST">
        <label for="depth">Depth:</label>
        <input type="number" name="depth" value="11" min="6" required><br><br>
        <label for="fen">FEN (or leave empty to use ./start.fen):</label><br>
        <textarea name="fen" rows="10" cols="50"></textarea><br>
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
        fen = request.form['fen'].strip()
        
        # Write FEN to ./start.fen if provided
        if fen:
            with open('start.fen', 'w') as f:
                f.write(fen)
        
        # Run Baeagn engine
        cmd = ['./baeagn', depth]
        process = subprocess.run(cmd, capture_output=True, text=True)
        result = process.stdout
    
    return render_template_string(HTML, result=result)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
