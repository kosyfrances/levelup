# daemon

This example illustrates how to write and work with linux daemons.

* (Part 1) Write and manage a SystemV daemon.
* (Part 2) Write and manage a Systemd daemon.

## Part 1

TODO: SysV style daemon

* Read description from reference link below.
* Run `make test-part1` and verify the tests pass.
* Inspect the `vagrant/test.sh` and `part1/*` files.
* Write your own implementation of the `fibd` daemon in either Go or Python.
* Update the `vagrant/test.sh` and `part1/Makefile` files to use your implementation (but do not modify the tests).
* Run `make test-part1` and verify the tests pass.

## Part 2

TODO: new-style daemon

* Read description from reference link below.
* Run `make test-part2` and verify the tests pass.
* Inspect the `vagrant/test.sh` and `part2/*` files.
* Write your own implementation of the `fibd` daemon in either Go or Python.
* Update the `vagrant/test.sh` and `part2/Makefile` files to use your implementation (but do not modify the tests).
* Run `make test-part2` and verify the tests pass.

## References

* https://www.freedesktop.org/software/systemd/man/daemon.html
