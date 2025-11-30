#! /usr/bin/bash

SOURCE=baeagn.c

if [ x"$_ICCF" = x1 ]; then
  SOURCE=iccf.c
fi

gcc -o baeagn \
    $SOURCE \
    -lm \
    -O4 \
    -march=native \
    -w \
    -D_CHESS960=$_CHESS960 \
    -D_NOEDIT=$_NOEDIT || exit 3
