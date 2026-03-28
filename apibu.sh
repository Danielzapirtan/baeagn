#! /usr/bin/bash

_CHESS960=0
SOURCE=api.c
: ${gamesymbol:=ini}

if [ x"$_ICCF" = x1 ]; then
  SOURCE=api.c
fi

gcc -o baeagn-api \
    $SOURCE \
    -lm \
    -O3 \
    -march=native \
    -w \
    -D$gamesymbol=1 \
    -D_OPTIMIZE=1 \
    -D_ICCF=$_ICCF \
    -D_CHESS960=$_CHESS960 \
    -D_NOEDIT=$_NOEDIT || exit 3

