#! /bin/bash

PGN=$1
DEPTH=$2
echo "$PGN" $DEPTH
ulimit -t 21000
rm -r start.pgn
echo "$PGN" >start.pgn
baeagn $DEPTH | tee start.d$DEPTH.anl

