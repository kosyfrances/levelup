#!/bin/bash

# update system packages
apt-get -y update
apt-get -y dist-upgrade

# install development tools
apt-get -y install build-essential
