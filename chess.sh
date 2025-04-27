#! /bin/bash

tempfile=/tmp/gh.tmp
username=antoniudanielzapirtan
url=https://api.chess.com/pub/player/$username/games/to-move

date +%H:%M
gh run list | grep in_progress >$tempfile
cat $tempfile | tail -n 6
cat $tempfile | wc  -l
curl $url 2>/dev/null | sed -e "s/daily/\n/g" | wc -l

