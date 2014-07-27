#!/bin/bash

if [[ `whoami` != "root" ]]; then
  echo "this script should run as root"
  exit 1
fi

if [[ -e /usr/bin/apt-get ]]; then
  apt-get install python2-setuptools python2-pip libgit2-dev
elif [[ -e /usr/bin/pacman ]]; then
  pacman -S python2-setuptools python2-pip libgit2
fi


cd /opt

rm -rfv /opt/phpcs_git_hook
git clone https://github.com/wusuopu/phpcs_git_hook

cd /opt/phpcs_git_hook

make
make install

tar xf phpcs.tar.gz

echo "install finished..."
