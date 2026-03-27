#! /bin/bash

export TZ=Europe/Bucharest
export PLAYER="$1"
export GLIST=pgn/gamelist.txt

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

