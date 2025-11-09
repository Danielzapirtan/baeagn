#! /bin/sh

user=antoniudanielzapirtan
NPROCESSORS=4
url1="https://api.chess.com/pub/player/$user/games/to-move"
curl -s $url1 >/tmp/games1.txt
COUNT=$(jq '.games | length' /tmp/games1.txt)
if [ $COUNT -gt $NPROCESSORS ]; then
	COUNT=$NPROCESSORS
fi
SESSION_TIME=1500
if true; then
if [ $COUNT -lt $NPROCESSORS ]; then
	NPROCESSORS=$COUNT
fi
fi
myltg=$(($SESSION_TIME * $NPROCESSORS / $COUNT))

cat bench.yml \
	| sed -e "s/mygn/$mygn/g" \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/count/$COUNT/g" \
	>.github/workflows/bench.yml
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh wf >~/$date.txt 2>&1 &
echo "$COUNT diagrams"
echo "All workflows triggered"

