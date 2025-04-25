#! /bin/bash

GAMESYMBOL=$1

gh workflow run play_move.yml -f pgn="$(cat pgn/$GAMESYMBOL.pgn)" --ref lesson

