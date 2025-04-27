import requests

# Replace with your Chess.com API URL
API_URL = "https://api.chess.com/pub/player/YOUR_USERNAME/games"

response = requests.get(API_URL)
if response.status_code == 200:
    games = response.json().get("games", [])
    for game in games:
        print(game.get("pgn", "No PGN found"))
else:
    print(f"Error fetching data: {response.status_code}")