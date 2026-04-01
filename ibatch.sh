#! /bin/bash

export TZ=Europe/Bucharest
export PLAYER="$1"
export GLIST=pgn/gamelist.txt
export STM=360

if true; then
date
: > $GLIST
for c in $COLORS; do
	GAME=$PLAYER$c
	echo $GAME
	cat - > pgn/$GAME.pgn
	echo $GAME >> $GLIST
done
fi

git add .
git commit -miccf
git push
gh workflow run iccf

echo "Workflow triggered ok"

