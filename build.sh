#! /usr/bin/bash

clang -o baeagn baeagn.c -lm -O4 -march=native -w  || exit 2
