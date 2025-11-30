#! /bin/bash

PGNFILE=base/sea.pgn
var="$1"

movelist=$(cat $PGNFILE \
  | grep -o "$var [^ ]\+ " \
  | sort -u | awk '{print $NF}')

for move in $movelist; do
  echo -n "$move "
  cat $PGNFILE | grep "$var $move "|wc -l
done|sort -nk 2
