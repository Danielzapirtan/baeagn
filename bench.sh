#! /bin/bash

BOARD=$1
DEPTH=$2
echo $BOARD $DEPTH
ulimit -t 21000
ln -sf pgn/$BOARD.pgn start.pgn
baeagn $DEPTH | tee $BOARD.d$DEPTH.anl

