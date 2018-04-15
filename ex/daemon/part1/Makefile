# project settings
TOP := $(shell pwd)

.PHONY: fibd clean install uninstall

# build the C daemon
# fibd: fibd.c
# 	gcc -o $(TOP)/fibd $(TOP)/fibd.c

# make the python daemon executable
fibd:
	cp fibd.py fibd
	chmod +x fibd

# remove build artifacts
clean:
	rm -f $(TOP)/fibd

# install the binary and related files
install:
	install -o root -g root -m 0755 $(TOP)/fibd         /usr/bin/fibd
	install -o root -g root -m 0644 $(TOP)/fibd.service /etc/systemd/system/fibd.service

# remove the binary and related files
uninstall:
	rm -f /usr/bin/fibd
	rm -f /etc/systemd/system/fibd.service
