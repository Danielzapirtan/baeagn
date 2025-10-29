#! /usr/bin/bash

gcc -o baeagn \
    baeagn.c \
    -lm \
    -O4 \
    -march=native \
    -w \
    -D_NOEDIT $_NOEDIT
    -D_GAME_ini || exit 2
