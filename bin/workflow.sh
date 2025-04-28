#! /bin/bash

GAMESYMBOL=$1

gh workflow run play_move.yml -f game_symbol=$GAMESYMBOL --ref main

