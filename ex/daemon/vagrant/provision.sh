#!/bin/bash

# update system packages
apt-get -y update
apt-get -y dist-upgrade

# install python for running ansible
apt-get -y install python

# install development tools
apt-get -y install build-essential
