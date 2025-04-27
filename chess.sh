#! /bin/bash

tempfile=/tmp/gh.tmp
username=antoniudanielzapirtan
url=https://api.chess.com/pub/player/$username/games

date +%H:%M
gh run list | grep in_progress >$tempfile
cat $tempfile | tail -n 6
cat $tempfile | wc  -l
curl "$url" 2>/dev/null | jq -r '.games[].turn' > /tmp/turn.txt
for n in $(seq 1 $(cat /tmp/turn.txt|wc -l)); do
	turn=$(cat /tmp/turn.txt | head -n $n | tail -n 1)
	case $turn in
white)
	who=$(curl "$url" 2>/dev/null | jq -r '.games[].white' | head -n $n|tail -n 1)
	;;
*)	
	who=$(curl "$url" 2>/dev/null | jq -r '.games[].black' | head -n $n|tail -n 1)
	;;
esac
who=$(echo $who | grep -o "[[:alnum:]]\+$")
	if echo $who | grep -iq $username; then
		echo -n "Game $n\t"
		echo -n "$(curl "$url" 2>/dev/null | jq -r '.games[].white' | head -n $n|tail -n 1 | grep -o "[[:alnum:]]\+$") - "
		curl "$url" 2>/dev/null | jq -r '.games[].black' | head -n $n|tail -n 1 | grep -o "[[:alnum:]]\+$"
		echo
	fi
done

