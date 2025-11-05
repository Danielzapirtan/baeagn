#! /bin/sh

COUNT=$1
SESSION_TIME=$2
myltg=$SESSION_TIME

cat bench.yml \
	| sed -e "s/mygn/$mygn/g" \
	>$HOME/bench.yml
cat $HOME/bench.yml \
	| sed -e "s/myltg/$myltg/g" \
	| sed -e "s/count/$COUNT/g" \
	>.github/workflows/bench.yml
date=$(date +%Y%m%d-%H%M%S)
echo $date
sh wf >~/$date.txt 2>&1 &
echo "All workflows triggered"

