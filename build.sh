#! /usr/bin/bash

clang -o baeagn baeagn.c -lm -O4 -march=native -w  || exit 2
sudo install -m 0755 -s baeagn /usr/local/bin
