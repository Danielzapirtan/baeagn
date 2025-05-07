#! /bin/bash

TIMELIMIT=$1
GAMESYMBOL=$2
echo $GAMESYMBOL $TIMELIMIT
rm -rf start.pgn
cp pgn/$GAMESYMBOL.pgn start.pgn
python3 app.py "$(cat start.pgn)" $TIMELIMIT | tee $GAMESYMBOL.anl

