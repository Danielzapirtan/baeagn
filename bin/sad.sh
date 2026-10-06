#! /usr/bin/bash

DATE=$(date +%Y%m%d_%H%M%S)
DIR=anl
TARGET=$HOME/anl_$DATE.tar.gz

cd $HOME/code/*/baeagn
mkdir -p /tmp/$DIR
cp -a [0-9]*.txt /tmp/$DIR
tar -C /tmp -czf $TARGET $DIR
rclone copy $TARGET drive:/$DIR
