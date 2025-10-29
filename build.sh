#! /usr/bin/bash

_NOEDIT=1
gcc -o baeagn \
    baeagn.c \
    -lm \
    -O4 \
    -march=native \
    -w \
    -D_NOEDIT=1 \
    -D_GAME_ini || exit 2
