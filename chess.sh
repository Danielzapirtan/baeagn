#! /bin/bash

tempfile=/tmp/gh.tmp
username=antoniudanielzapirtan
url=https://api.chess.com/pub/player/$username/games/to-move

date +%H:%M
gh run list | grep in_progress >$tempfile
cat $tempfile | tail -n 6
cat $tempfile | wc  -l
curl $url 2>/dev/null | sed -e "s/daily/\n/g" | wc -l

# Replace with your Chess.com API URL
API_URL="https://api.chess.com/pub/player/antoniudanielzapirtan/games"

#curl -s "$API_URL" | jq -r '.games[].pgn'

# Fetch JSON data and process games where it's your turn
curl -s "$API_URL" | jq -r '
  .games[] | 
  select(.turn == "antoniudanielzapirtan") | 
  "[\(.opponent.username), \(.color)]"
'