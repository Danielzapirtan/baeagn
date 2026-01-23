#! /usr/bin/bash

SOURCE=baeagn.c
: ${gamesymbol:=ini}

if [ x"$_ICCF" = x1 ]; then
  SOURCE=iccf.c
fi

gcc -o baeagn \
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

