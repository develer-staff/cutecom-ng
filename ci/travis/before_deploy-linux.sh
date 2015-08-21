#!/bin/bash

echo
echo Running before_deploy-linux.sh...
echo

fpm -s dir -t deb -n cutecom-ng -v "$TRAVIS_TAG" -d libqt5serialport5 -d libqt5widgets5 -d libqt5core5a -d libqt5gui5 bin/
dpkg --contents *.deb
export RELEASE_PKG_FILE=$(ls *.deb)
echo "deploying $RELEASE_PKG_FILE to GitHub releases"
