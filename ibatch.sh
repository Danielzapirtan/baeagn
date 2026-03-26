#! /bin/bash

export TZ=Europe/Bucharest
git add .
git commit -miccf
git push
gh workflow run iccf
(sleep 18000; /data/data/com.termux/files/usr/bin/termux-notification --title "Baeagn/ICCF" --content "Analyzed ok @ $(date +%H:%M)") &
echo "Workflow triggered ok"

