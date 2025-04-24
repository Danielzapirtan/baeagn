#! /bin/bash

PGN="$1"
DEPTH=$2
echo "$PGN" $DEPTH
ulimit -t 21000
rm -r start.pgn
cp input/game.pgn start.pgn
baeagn $DEPTH | tee start.d$DEPTH.anl

