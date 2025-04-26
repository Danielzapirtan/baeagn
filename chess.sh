#! /bin/bash

username=antoniudanielzapirtan
url=https://api.chess.com/pub/player/$username/games/to-move

gh run list --status in_progress | wc  -l
curl $url 2>/dev/null | grep daily | wc -l

