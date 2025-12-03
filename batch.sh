#! /bin/bash

cd $HOME/*/baeagn

user=antoniudanielzapirtan
NPROCESSORS=3
url1="https://api.chess.com/pub/player/$user/games/to-move"
curl -s $url1 >$HOME/games1.txt
COUNT=$(jq '.games | length' $HOME/games1.txt)
COUNTF=$COUNT
SESSION_TIME=21000
REMAINING=$COUNT
ECART=0

while true; do
if [ $REMAINING -lt 1 ]; then
	echo "$COUNTF diagrams"
	echo "All workflows triggered"
	exit
fi
if [ $REMAINING -lt $NPROCESSORS ]; then
  NPROCESSORS=$REMAINING
fi
REMAINING=$(($REMAINING - $NPROCESSORS))
COUNT=$NPROCESSORS
myltg=$SESSION_TIME

cat bench.yml \
  | sed -e "s/count/$COUNT/g" \
	| sed -e "s/mygn/$mygn/g" \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/ecart/$ECART/g" \
	>.github/workflows/bench.yml
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh wf >~/$date.txt 2>&1 &
sleep 5
ECART=$(($ECART + $NPROCESSORS))
done

