#! /bin/bash

COMMON=$(cat message)
DATE=$(date +%y%m%d-%H%M)

: ${MESSAGE:="$DATE $COMMON $1"}

git add .
git commit -m "$MESSAGE"
git push

echo "Commited ok $MESSAGE"
