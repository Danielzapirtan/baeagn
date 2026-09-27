#! /bin/bash

_ICCF=1 _CHESS960=0 _NOEDIT=1 bash bin/build.sh
sudo install -m 0755 baeagn /usr/local/bin
(baeagn &>anl/$(date +%y%m%d-%H%M).anl &)
