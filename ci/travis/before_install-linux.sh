#!/bin/bash

echo
echo Running before_install-linux.sh...
echo

sudo add-apt-repository --yes ppa:beineri/opt-qt55
sudo apt-get update -qq
sudo apt-get install -qq qt55base qt55serialport qt55tools
source /opt/qt55/bin/qt55-env.sh
gem install fpm
