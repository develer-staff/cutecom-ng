#!/bin/bash

echo
echo Running before_install-linux.sh...
echo

sudo add-apt-repository --yes ppa:beineri/opt-qt541
sudo apt-get update -qq
sudo apt-get install -qq qt54base qt54serialport qt54tools
source /opt/qt54/bin/qt54-env.sh
gem install fpm
