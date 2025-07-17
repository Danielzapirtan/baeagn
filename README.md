# baeagn
Baeagn chess engine 

Author: Danielzapirtan

This file is somehow outdated.
The code may have different behaviour.
I write chess programs since 1988.
This is the current one for the CLI.
It is written in C.
With
    sh run
analyze the board in start.brd (a custom file format),
then put the engine's log in
    /usr/local/share/baeagn/anl/start.anl
.
You can view the engine's log with
    sh list
The executable is compiled and installed by
    sh build.sh
as
    /usr/local/bin/baeagn
.
It is handy to put symlinks in
    /usr/local/bin
to baeagn, for example
    cd /usr/local/bin
    ln -sf baeagn ini
if we analyze the initial position (ini.brd).

