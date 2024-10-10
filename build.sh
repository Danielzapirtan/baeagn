#! /usr/bin/bash

gcc -o baeagn baeagn.c -lm -O4 -march=native -w || exit 2
install -m 0755 -s baeagn /usr/local/bin
rm -rf baeagn
