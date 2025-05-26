#! /bin/bash

tempfile=/tmp/gh.tmp
username=antoniudanielzapirtan
url=https://api.chess.com/pub/player/$username/games

date +%H:%M
gh run list | grep in_progress >$tempfile
cat $tempfile | tail -n 6 | cut -f 1,7-9
k=$(cat $tempfile | wc  -l)
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
		echo -en "Game $n\t"
		echo -n "$(curl "$url" 2>/dev/null | jq -r '.games[].white' | head -n $n|tail -n 1 | grep -o "[[:alnum:]]\+$") - "
		curl "$url" 2>/dev/null | jq -r '.games[].black' | head -n $n|tail -n 1 | grep -o "[[:alnum:]]\+$"
	fi
done

if echo $k|grep -qv "^0$"; then
for id in $(gh run list -L $k | cut -f 7); do
	for job in $(gh run view $id | grep -o "\<4[0-9]\{10\}\>"|head -n 1); do
		gh run view --job $job
	done;
done | grep "Run benchmarks" | grep -o "\<[a-z]\{4\}$" | fmt
fi

