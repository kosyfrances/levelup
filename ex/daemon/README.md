# daemon

This example illustrates how to write and work with linux daemons.

* (Part 1) Write and manage a SystemV daemon.
* (Part 2) Write and manage a Systemd daemon.

## Background

TODO: processes and daemons

* Read the reference material [1].

## Part 1

TODO: SysV style daemon

1. Read the relevant section in the reference material [2].
2. Run `make test-part1` and verify the tests pass.
3. Inspect the `vagrant/test.sh` and `part1/*` files.
4. Write your own implementation of the `fibd` daemon in either Go or Python.
5. Update the `vagrant/test.sh` and `part1/Makefile` files to use your implementation (but do not modify the tests).
6. Run `make test-part1` and verify the tests pass.

## Part 2

TODO: new-style daemon

1. Read the relevant section in the reference material [2].
2. Run `make test-part2` and verify the tests pass.
3. Inspect the `vagrant/test.sh` and `part2/*` files.
4. Write your own implementation of the `fibd` daemon in either Go or Python.
5. Update the `vagrant/test.sh` and `part2/Makefile` files to use your implementation (but do not modify the tests).
6. Run `make test-part2` and verify the tests pass.

## References

1. http://www.tldp.org/LDP/tlk/kernel/processes.html
2. https://www.freedesktop.org/software/systemd/man/daemon.html
