#!/usr/bin/env python3

"""
An example SystemD daemon.
"""

from time import sleep
from signal import signal, SIGHUP, SIGTERM, SIGINT

# Whether the daemon should continue running.
running = False

def handle_signal(signum, frame):
    """sent by process manager to indicate we should reload

    Arguments:
        signum {int} -- sent by process manager to indicate we should reload
    """
    other_signals = [SIGTERM, SIGINT]
    if signum == SIGHUP:
        # reload daemon configuration files and reopen file descriptors
        print("ignoring SIGHUP because there is nothing to do\n")
    elif signum in other_signals:
        # SIGTERM is sent by process manager to indicate we should stop gracefully
        # SIGINT is sent by user hitting Ctrl + C in the terminal
        signal_name = other_signals[other_signals.index(signum)].name
        print("received {} and clearing the run flag\n".format(signal_name))
        global running
        running = False


if __name__ == "__main__":
    # set the running flag
    running = True

    # register the signal handler for relevant signals
    signal(SIGHUP, handle_signal)
    signal(SIGTERM, handle_signal)
    signal(SIGINT, handle_signal)

    # report our status
    print("starting\n")

    # do useful work
    a = 0
    b = 1

    while running:
        # sleep for a while
        sleep(1)

        # compute the next term
        sum = a + b
        a = b
        b = sum

        # report the term
        print("computed term: {}\n".format(a))

        # Break the loop if running changes to false
        if not running:
            break

    # report our status
    print("stopping\n")

    # exit cleanly
    exit(0)
