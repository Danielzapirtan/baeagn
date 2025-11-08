#! /bin/sh

COUNT=$1
SESSION_TIME=$2
NPROCESSORS=4
if true; then
if [ $COUNT -lt 4 ]; then
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
echo "All workflows triggered"

