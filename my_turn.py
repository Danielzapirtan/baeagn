import requests

username = "YourExactUsername"  # ← Replace with your real username (case-sensitive!)
url = f"https://api.chess.com/pub/player/{username}/games"

def is_my_turn(game, username):
    fen = game.get("fen", "")
    active_color = fen.split(" ")[1] if fen else ""
    if active_color == "w" and game.get("white", {}).get("username", "").lower() == username.lower():
        return True
    if active_color == "b" and game.get("black", {}).get("username", "").lower() == username.lower():
        return True
    return False

def describe_game(game, username):
    white = game.get("white", {}).get("username", "Unknown")
    black = game.get("black", {}).get("username", "Unknown")
    time_control = game.get("time_control", "Unknown")
    game_url = game.get("url", "")

    if white.lower() == username.lower():
        opponent = black
        color = "White"
    else:
        opponent = white
        color = "Black"

    return f"{color} vs {opponent} | Time control: {time_control} | {game_url}"

try:
    response = requests.get(url)
    if response.status_code == 200:
        games = response.json().get("games", [])
        my_turn_games = [game for game in games if is_my_turn(game, username)]

        if my_turn_games:
            print(f"Games where it's your move ({len(my_turn_games)}):\n")
            for game in my_turn_games:
                print("- " + describe_game(game, username))
        else:
            print("No games where it's your move.")
    elif response.status_code == 403:
        print("Error 403: Check username spelling or privacy settings!")
    elif response.status_code == 429:
        print("Too many requests! Wait and retry later.")
    else:
        print(f"Unexpected error: HTTP {response.status_code}")
except Exception as e:
    print(f"Request failed: {e}")