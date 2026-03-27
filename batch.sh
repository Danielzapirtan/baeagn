#! /usr/bin/bash

cd $HOME/git-client/baeagn

export TZ=Europe/Bucharest
user=antoniudanielzapirtan
NPROCESSORS=15
url1="https://api.chess.com/pub/player/$user/games/to-move"
curl -s "$url1" >$HOME/games1.txt
COUNT=$(jq '.games | length' $HOME/games1.txt)
COUNTF=$COUNT
PAR=4
ST=1500
if [ $COUNTF -lt $PAR ]; then
	PAR=$COUNTF
fi
SESSION_TIME=$(($ST*$PAR/$COUNTF))
REMAINING=$COUNT
ECART=0

while true; do
if [ $REMAINING -lt 1 ]; then
  echo "$COUNTF diagrams"
  echo "$SESSION_TIME"
  echo "All workflows triggered"
  STM=$(($ST/60))
  HOUR=$(date -d "+$STM minutes" +%H)
  MINUTES=$(date -d "+$STM minutes" +%M)
  DOM=$(date -d "+$STM minutes" +%d)
  TITLE="Baeagn on Chess"
  CONTENT="Analysis completed at $HOUR:$MINUTES"
  NEW_RECORD="$MINUTES $HOUR $DOM * * termux-notification --title \"$TITLE\" --content \"$CONTENT\""
  (crontab -l 2>/dev/null; echo "$NEW_RECORD") | crontab -
  echo "Added $NEW_RECORD to crontab"
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
