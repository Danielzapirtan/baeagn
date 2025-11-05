#! /bin/bash

user=antoniudanielzapirtan
GN=$1

url1="https://api.chess.com/pub/player/$user/games/to-move"
url="https://api.chess.com/pub/player/$user/games"

set -e

if true; then
curl -s $url1 >/tmp/games1.txt
curl -s $url >/tmp/games.txt
fi

echo $GN
myurl=$(jq -c .games[$GN].url /tmp/games1.txt)

n=0
while true; do
	myurlc=$(jq -c .games[$n].url /tmp/games.txt)
	if [ x"$myurl" = x"$myurlc" ]; then
		fen=$(jq -r .games[$n].fen /tmp/games.txt)
		echo $fen >start.fen
		jq -c .games[$n].white /tmp/games.txt
		jq -c .games[$n].black /tmp/games.txt
		jq -c .games[$n].rules /tmp/games.txt
		exit 0
	fi
	n=$(($n + 1))
done
