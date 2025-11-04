#! /bin/sh

COUNT=$1

for n in $(seq 1 $COUNT); do
	mygn=$(($n - 1))
	cat bench.yml \
		| sed -e "s/mygn/$mygn/g" \
		>.github/workflows/bench.yml
	date=$(date +%Y%m%d-%H%M)
	echo $date
	sh wf 1 >~/$date.txt 2>&1 &
	sleep 120
done
echo "All workflows triggered"

