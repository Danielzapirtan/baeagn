#! /bin/bash

user=antoniudanielzapirtan

url1="https://api.chess.com/pub/player/$user/games/to-move"
url="https://api.chess.com/pub/player/$user/games"

set -e

if true; then
curl -s $url1 >/tmp/games1.txt
curl -s $url >/tmp/games.txt
fi

myurl=$(jq -c .games[0].url /tmp/games1.txt)
echo $myurl
n=0
while true; do
	myurlc=$(jq -c .games[$n].url /tmp/games.txt)
	if [ x"$myurl" = x"$myurlc" ]; then
		jq -c .games[$n].pgn /tmp/games.txt | \
			grep "[[:digit:]\+\.\+ [[:alnum:]+-]\+ " \
			>/tmp/mygame.pgn
		        echo "*" >>/tmp/mygame.pgn
		exit 0
	fi
	n=$(($n + 1))
done

