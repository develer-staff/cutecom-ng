#!/bin/bash

echo
echo Running before_install-osx
echo

brew update > /dev/null
brew install qt5
brew linkapps qt5
brew install python > /dev/null
