# daemon

This example illustrates how to write and work with linux daemons.

* (Part 1) Write and manage a SystemV daemon.
* (Part 2) Write and manage a Systemd daemon.

## Background

In general there are three layers used to build applications. From the lowest to the highest level:

* Thread. A thread is a set of instructions and local memory. (i.e. GUI thread, spell check thread, etc)
* Process. One or more threads with shared process memory. (i.e. Word process, OneDrive sync process, etc)
* Application. One or more processes working together to achieve a task. (i.e. Microsoft Word application)

A daemon is a long-running process that performs long-running or periodic work in the background. This is opposed to a typical application process that is only running while in use by the user. Each operating system has it's own specific way to create, run, and manage daemons. This example focuses on Linux daemons. More information on processes and how they work can be found in the Linux kernel documentation [1].

In Linux there is a concept of an init process. The init process is the first process the kernel runs after booting and is primarily responsible for managing all other processes run while the operating system is running. Part of that task involves managing long-running daemon processes. Until recently this was implemented by the SystemV project. In modern Linux distributions however SystemD is typically used in place of SystemV. Both of these accomplish the same goal: providing an init binary for the kernel to run and a set of utilities for communicating with it in order to manage running processes. Each system has it's own way for working with daemons so this example will demonstrate how to write a daemon that works with each.

## Part 1

The goal of this part is to write a SystemV compatible daemon. An example one that computes the Fibonacci sequence is provided. The example is written in C but any process that behaves according to SystemV's rules will work. It doesn't matter what language the binary is written in as long as it follows the rules. Unfortunately, is is difficult and inconvenient to run two different init systems at the same time. The Ubuntu VM configured by Vagrant for this project uses SystemD as it's init system. Thankfully, SystemD can be configured to use the same set of rules that SystemV normally uses.

1. Read the `SysV Daemons` section in the SystemD documentation [2].
2. Run `make test-part1` and verify the tests pass.
3. Inspect the `part1.yaml` Ansible playbook, and `part1/fibd.c` implementation, and `part1/fibd.service` SystemD configuration file.
4. Write your own implementation of the `fibd` daemon in your language of choice.
5. Update the relevant files to use your implementation instead of the provided example (but do not modify the tests).
6. Run `make test-part1` and verify the tests pass.

## Part 2

TODO: new-style daemon

1. Read the `New-Style Daemons` section in the SystemD documentation [2].
2. Run `make test-part2` and verify the tests pass.
3. Inspect the `part2.yaml` Ansible playbook, and `part2/fibd.c` implementation, and `part2/fibd.service` SystemD configuration file.
4. Write your own implementation of the `fibd` daemon in your language of choice.
5. Update the relevant files to use your implementation instead of the provided example (but do not modify the tests).
6. Run `make test-part2` and verify the tests pass.

## References

1. http://www.tldp.org/LDP/tlk/kernel/processes.html
2. https://www.freedesktop.org/software/systemd/man/daemon.html
