#! /bin/bash

export TZ=Europe/Bucharest
git add .
git commit -miccf
git push
gh workflow run iccf
echo "Workflow triggered ok"

