#! /bin/bash

DEPTH=$1
GAMESYMBOL=$2
echo $GAMESYMBOL $DEPTH
rm -rf start.pgn
cp pgn/$GAMESYMBOL.pgn start.pgn
python3 app.py "$(cat start.pgn)"

