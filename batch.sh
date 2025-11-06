#! /bin/sh

COUNT=$1
SESSION_TIME=$2
NPROCESSORS=8
myltg=$(($SESSION_TIME * $NPROCESSORS / (1 + $COUNT)))

cat bench.yml \
	| sed -e "s/mygn/$mygn/g" \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/count/$COUNT/g" \
	>.github/workflows/bench.yml
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh wf >~/$date.txt 2>&1 &
echo "All workflows triggered"

