#! /bin/bash

username=antoniudanielzapirtan
url=https://api.chess.com/pub/player/$username/games/to-move

date +%H:%M
gh run list | head -n 2 | cut -f 8
gh run list --status in_progress | wc  -l
curl $url 2>/dev/null | sed -e "s/daily/\n/g" | wc -l

