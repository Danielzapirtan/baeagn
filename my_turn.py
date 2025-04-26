import requests

username = "AntoniuDanielZapirtan"
url = f"https://api.chess.com/pub/player/{username}/games/to-move"

response = requests.get(url)
if response.status_code == 200:
    games = response.json()["games"]
    print(f"Games where it's your turn: {games}")
else:
    print("Error fetching games:", response.status_code)
