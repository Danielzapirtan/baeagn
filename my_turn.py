import requests
import time

username = "AntoniuDanielZapirtan"  # ← Case-sensitive!
url = f"https://api.chess.com/pub/player/{username}/games/to-move"

try:
    response = requests.get(url)
    if response.status_code == 200:
        games = response.json().get("games", [])
        print(f"Games to move: {games}")
    elif response.status_code == 403:
        print("Error 403: Check username spelling or privacy settings!")
    elif response.status_code == 429:
        print("Too many requests! Wait and retry later.")
    else:
        print(f"Unexpected error: HTTP {response.status_code}")
except Exception as e:
    print(f"Request failed: {e}")