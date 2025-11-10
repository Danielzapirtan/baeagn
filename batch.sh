#! /bin/sh

user=antoniudanielzapirtan
NPROCESSORS=4
url1="https://api.chess.com/pub/player/$user/games/to-move"
curl -s $url1 >/tmp/games1.txt
COUNT=$(jq '.games | length' /tmp/games1.txt)
SESSION_TIME=21000
REMAINING=$COUNT
ECART=0

while true; do
if [ $REMAINING -lt $NPROCESSORS ]; then
	exit
fi
REMAINING=$(($REMAINING - $NPROCESSORS))
COUNT=$NPROCESSORS
myltg=$SESSION_TIME

cat bench.yml \
	| sed -e "s/mygn/$mygn/g" \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/ecart/$ECART/g" \
	>.github/workflows/bench.yml
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh wf >~/$date.txt 2>&1 &
echo "$COUNT diagrams"
sleep 5
ECART=$(($ECART + 4))
done
echo "All workflows triggered"

