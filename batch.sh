#! /bin/sh

COUNT=$1
myltg=$2
TZ=EET
export TZ

for n in $(seq 1 $COUNT); do
	mygn=$(($n - 1))
	cat bench.yml \
		| sed -e "s/mygn/$mygn/g" \
		>/tmp/bench.yml
	cat /tmp/bench.yml \
		| sed -e "s/myltg/$myltg/g" \
		>.github/workflows/bench.yml
	date=$(date +%Y%m%d-%H%M)
	echo $date
	sh wf 1 >~/$date.txt 2>&1 &
	sleep 60
done
echo "All workflows triggered"

