#! /bin/bash

PGNFILE=base/sea.pgn

movelist=$(cat $PGNFILE \
  | grep -o "^6\.Be3 e5 [^ ]\+ " \
  | sort -u | awk '{print $3}')

for move in $movelist; do
  echo -n "$move "
  cat $PGNFILE | grep "^6\.Be3 e5 $move "|wc -l
done|sort -nk 2
