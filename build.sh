#! /usr/bin/bash

gcc -o baeagn \
    baeagn.c \
    -lm \
    -O4 \
    -march=native \
    -w \
    -DGAME_ini || exit 2
