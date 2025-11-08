#! /bin/sh

COUNT=$1
SESSION_TIME=$2
NPROCESSORS=8
if [ $COUNT -lt 7 ]; then
	NPROCESSORS=$((1 + $COUNT))
fi
myltg=$(($SESSION_TIME * $NPROCESSORS / 2 / (1 + $COUNT)))

cat bench.yml \
	| sed -e "s/mygn/$mygn/g" \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/count/$COUNT/g" \
	>.github/workflows/bench.yml
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh wf >~/$date.txt &
echo "All workflows triggered"

