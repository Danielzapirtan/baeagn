#! /bin/bash

DEPTH=$1
GAMESYMBOL=$2
echo $GAMESYMBOL $DEPTH
ulimit -t 21000
rm -rf start.pgn
cp pgn/$GAMESYMBOL.pgn start.pgn
baeagn $DEPTH | tee $GAMESYMBOL.d$DEPTH.anl

