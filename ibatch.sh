#! /bin/bash

git add .
git commit -miccf
git push
gh workflow run iccf
(sleep 18000; termux-notification --title "Baeagn/ICCF" --content "Analyzed ok") &
echo "Workflow triggered ok"

