#! /usr/bin/bash

clang -o baeagn baeagn.cpp -lm -O4 -march=native -w  || exit 2
