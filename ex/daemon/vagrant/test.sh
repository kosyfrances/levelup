#!/bin/bash

# stop systemd service if present
if [ -f "/etc/systemd/system/fibd.service" ]; then
  systemctl stop fibd.service
fi

# stop stray daemon processes
killall fibd || true &> /dev/null

# build and install the provided sample
pushd /vagrant/${PROJECT} &> /dev/null
make
make install
popd &> /dev/null

# reload systemctl service configuration
systemctl daemon-reload
