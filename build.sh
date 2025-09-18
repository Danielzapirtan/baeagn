#! /usr/bin/bash

gcc -o baeagn \
    baeagn.c \
    -lm \
    -O4 \
    -march=native \
    -w \
    -D_GAME_angw || exit 2
