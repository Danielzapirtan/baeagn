#! /bin/bash

PGN="$1"
DEPTH=$2
GAMESYMBOL=$3
echo "$PGN" $DEPTH
ulimit -t 21000
rm -r start.pgn
cp input/game.pgn start.pgn
baeagn $DEPTH | tee $GAMESYMBOL.d$DEPTH.anl

