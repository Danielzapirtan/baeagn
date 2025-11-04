#! /bin/bash

COUNT=$1

for n in $(seq 1 $COUNT); do
	mygn=$(($n - 1))
	cat bench.yml \
		| sed -e "s/mygn/$mygn/g" \
		>.github/workflows/bench.yml
		date=$(date -d "+28 minutes" +%Y%m%d-%H%M)
		(sh wf 1 &>~/$date.txt &)
		sleep 120
done
echo "All workflows triggered"

