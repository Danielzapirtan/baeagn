#! /bin/bash

export TZ=Europe/Bucharest
export PLAYER="$1"
export GLIST=pgn/gamelist.txt
export STM=300

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

HOUR=$(date -d "+$STM minutes" +%H)
MINUTES=$(date -d "+$STM minutes" +%M)
DOM=$(date -d "+$STM minutes" +%d)

TITLE="Baeagn on ICCF $PLAYER ($COLORS)"
CONTENT="Analysis completed at $HOUR:$MINUTES"
COMMAND="termux-notification --title \"$TITLE\" --content \"$CONTENT\""
NEW_RECORD="$MINUTES $HOUR $DOM * * $COMMAND"
(crontab -l 2>/dev/null; echo "$NEW_RECORD") | crontab -
echo "Added $NEW_RECORD to crontab"

