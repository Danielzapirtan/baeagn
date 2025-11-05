#! /bin/sh

COUNT=$1
SESSION_TIME=86400
myltg=$(($SESSION_TIME / $COUNT))

for n in $(seq 1 $COUNT); do
	mygn=$(($n - 1))
	cat bench.yml \
		| sed -e "s/mygn/$mygn/g" \
		>$HOME/bench.yml
	cat $HOME/bench.yml \
		| sed -e "s/myltg/$myltg/g" \
		>.github/workflows/bench.yml
	date=$(date +%Y%m%d-%H%M%S)
	echo $date
	delay=$(($myltg + 60))
	sh wf 1 $delay >~/$date.txt 2>&1 &
	sleep 5
done
echo "All workflows triggered"

