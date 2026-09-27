#! /bin/bash

GN=$1

USERNAME=antoniudanielzapirtan
url1="https://api.chess.com/pub/player/$USERNAME/games/to-move"
url="https://api.chess.com/pub/player/$USERNAME/games"

set -e

if true; then
curl -fsSL --retry 3 --retry-delay 2 --connect-timeout 10 --max-time 30 \
	-A 'baeagn/1.0 (+https://github.com/Danielzapirtan/baeagn)' \
	-H 'Accept: application/json' "$url1" >/tmp/games1.txt
curl -fsSL --retry 3 --retry-delay 2 --connect-timeout 10 --max-time 30 \
	-A 'baeagn/1.0 (+https://github.com/Danielzapirtan/baeagn)' \
	-H 'Accept: application/json' "$url" >/tmp/games.txt
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
