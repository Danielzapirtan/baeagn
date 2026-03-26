#! /bin/bash

: ${MESSAGE:="$1"}

git add .
git commit -m "$MESSAGE"
git push

echo "Commited ok $MESSAGE"
