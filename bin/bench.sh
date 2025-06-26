#! /bin/bash

DEPTH=$1
GAMESYMBOL=$2
C7FLAG=$3
echo $GAMESYMBOL $DEPTH
rm -rf start.pgn
cp pgn/$GAMESYMBOL.pgn start.pgn
cat start.pgn
baeagn $DEPTH $C7FLAG | tee $GAMESYMBOL.d$DEPTH.anl

