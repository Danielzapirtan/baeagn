#! /usr/bin/bash

set -e

cd $HOME/code/*/baeagn

export TZ=Europe/Bucharest
export USERNAME=antoniudanielzapirtan
NPROCESSORS=4

#ping -c 1 8.8.8.8 &>/dev/null
#[ $? -eq 2 ] && exit 0
rm -rf [0-9]*.txt
pkill -15 baeagn
url1="https://api.chess.com/pub/player/$USERNAME/games/to-move"
curl -fsSL --retry 3 --retry-delay 2 --connect-timeout 10 --max-time 30 \
	-A 'baeagn/1.0 (+https://github.com/Danielzapirtan/baeagn)' \
	-H 'Accept: application/json' "$url1" >"$HOME/games1.txt"
COUNT=$(jq '.games | length' $HOME/games1.txt)
[ "x$COUNT" = "x" ] && exit 0
[ $COUNT -gt 0 ] || exit 0
if [ $COUNT -gt 8 ]; then
	COUNT=8
fi
COUNTF=$COUNT
PAR=4
ST=180
if [ $COUNTF -lt $PAR ]; then
	PAR=$COUNTF
fi
SESSION_TIME=$ST
REMAINING=$COUNT
ECART=0

while true; do
if [ $REMAINING -lt 1 ]; then
  echo "$COUNTF diagrams"
  echo "$SESSION_TIME"
  echo "All workflows triggered"
  exit
fi
if [ $REMAINING -lt $NPROCESSORS ]; then
  NPROCESSORS=$REMAINING
fi
REMAINING=$(($REMAINING - $NPROCESSORS))
COUNT=$NPROCESSORS
myltg=$SESSION_TIME

cat bin/cowf \
  | sed -e "s/count/$COUNT/g" \
  | sed -e "s/username/$USERNAME/g" \
	| sed -e "s/mygn/$mygn/g" \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/ecart/$ECART/g" \
	>bin/cowg
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh bin/cowg &
sleep 5
ECART=$(($ECART + $NPROCESSORS))
done
