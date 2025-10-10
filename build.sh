#! /usr/bin/bash

g++ -o baeagn \
    baeagn.cpp \
    -lm \
    -O4 \
    -march=native \
    -w \
    -D_GAME_ini || exit 2
