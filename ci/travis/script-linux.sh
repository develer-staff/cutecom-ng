#!/bin/bash

echo
echo Running script-linux.sh...
echo

qmake -v
qmake
make
ldd bin/cutecom-ng
