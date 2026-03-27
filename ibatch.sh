#! /bin/bash

export TZ=Europe/Bucharest
export PLAYER="$1"
export GLIST=pgn/gamelist.txt
export STM=300

date
: > $GLIST
for c in w b; do
	GAME=$PLAYER$c
	echo $GAME
	cat - > pgn/$GAME.pgn
	echo $GAME > $GLIST
done

git add .
git commit -miccf
git push
gh workflow run iccf

echo "Workflow triggered ok"

for n in a; do

for n in $(seq 1 $STM); do
	sleep 60
done

termux-notification \
	--title "Baeagn on ICCF" \
	--content "$GAME $DATE"

done &
