#! /usr/bin/bash

SOURCE=baeagn.c

gcc -o baeagn \
    $SOURCE \
    -lm \
    -O4 \
    -w \
    -D_CHESS960=0 \
    -D_NOEDIT=2 || exit 3

