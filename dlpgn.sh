#! /bin/bash

user=antoniudanielzapirtan

url1="https://api.chess.com/pub/player/$user/games/to-move"
url="https://api.chess.com/pub/player/$user/games"

set -e

if true; then
curl -s $url1 >/tmp/games1.txt
curl -s $url >/tmp/games.txt
fi

echo 1
myurl=$(jq -c .games[0].url /tmp/games1.txt)

echo 2
n=0
while true; do
	myurlc=$(jq -c .games[$n].url /tmp/games.txt)
	if [ x"$myurl" = x"$myurlc" ]; then
		jq -c .games[$n].pgn /tmp/games.txt | \
			grep -o "[[:digit:]]\+\.\+ [[:alnum:]+-=]\+ " \
			>pgn/bench.pgn
		        echo "*" >>pgn/bench.pgn
		jq -c .games[$n].white /tmp/games.txt
		jq -c .games[$n].black /tmp/games.txt
		exit 0
	fi
	n=$(($n + 1))
done

